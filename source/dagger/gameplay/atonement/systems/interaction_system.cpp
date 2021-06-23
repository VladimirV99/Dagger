#pragma once

#include "core/core.h"
#include "core/system.h"
#include "core/engine.h"

#include "core/game/transforms.h"
#include "core/graphics/sprite.h"
#include "core/input/inputs.h"
#include "gameplay/atonement/systems/interaction_system.h"
//#include "gameplay/atonement/systems/cooldown_manager.h"
#include "gameplay/atonement/components/marker_components.h"
#include "gameplay/atonement/systems/character_collisions.h"
#include "gameplay/atonement/atonement_controller.h"
#include "gameplay/atonement/systems/atonement_end_screen.h"
#include "gameplay/atonement/systems/atonement_pause_system.h"
#include <iostream>

using namespace dagger;
using namespace atonement;

void IntearactionSystem::SpinUp() 
{
    Engine::Dispatcher().sink<KeyboardEvent>().connect<&IntearactionSystem::OnKeyboardEvent>(this);
    //Create the 'Press E' sprite for interaction
    {
        auto& reg = Engine::Registry();
        auto entity = reg.create();
        auto& sprite = reg.emplace<Sprite>(entity);
        AssignSprite(sprite, "TextAssets:PressE");
        auto& transform = reg.emplace<Transform>(entity);
        transform.position = Vector3{ 0, 0 , -1 };    //-1 on Z axis so it is not visible on game start
        PressEPrompt = entity; 
    }

    //Create the dash tutorial prompt
    {
        auto& reg = Engine::Registry();
        auto entity = reg.create();
        auto& sprite = reg.emplace<Sprite>(entity);
        AssignSprite(sprite, "TextAssets:dash");
        auto& transform = reg.emplace<Transform>(entity);
        transform.position = Vector3{ 0, 0 , -1 };
        LShiftPrompt = entity;
    }

    //Create the dash tutorial prompt
    {
        auto& reg = Engine::Registry();
        auto entity = reg.create();
        auto& sprite = reg.emplace<Sprite>(entity);
        AssignSprite(sprite, "TextAssets:walljump");
        auto& transform = reg.emplace<Transform>(entity);
        transform.position = Vector3{ 0, 0 , -1 };
        WJPrompt = entity;
    }
}

void IntearactionSystem::WindDown()
{
    Engine::Dispatcher().sink<KeyboardEvent>().disconnect<&IntearactionSystem::OnKeyboardEvent>(this);
}

void IntearactionSystem::Run()
{   
    if (!loreStonePositionsLoaded) 
    {
        auto view = Engine::Registry().view<Transform, InteractableComponent>();
        for (auto& entity : view)
        {   
            auto& transf = view.get<Transform>(entity);
            auto& interact = view.get<InteractableComponent>(entity);
            loreStonePositions.push_back( std::make_pair(transf.position, interact.id) );
        }

        loreStonePositionsLoaded = true;
    }

    //a nonexistant value
    nearestLoreStone = std::make_pair(Vector3{0, 0, 0}, -1);
    interactionInputEnabled = false;

    auto playerView = Engine::Registry().view<Transform, CharacterCollision>();
    for (auto& entity : playerView) 
    {   
        auto& playerTransform = playerView.get<Transform>(entity);

        for (auto& stone : loreStonePositions) 
        {   
            if (std::abs(playerTransform.position.x - stone.first.x) < 250 &&
                std::abs(playerTransform.position.y - stone.first.y) < 150)
            {
                nearestLoreStone = stone;
                interactionInputEnabled = true;
            }
        }
        
        //if a stone is actually near
        if (interactionInputEnabled)
        {   
            //show prompts
            if (Engine::Registry().has<Transform>(PressEPrompt))
            {
                auto& interactPromptTransform = Engine::Registry().get<Transform>(PressEPrompt);
                interactPromptTransform.position.x = playerTransform.position.x;
                interactPromptTransform.position.y = playerTransform.position.y + 130;
                interactPromptTransform.position.z = 10;
            }

            if (Engine::Registry().has<Transform>(LShiftPrompt))
            {
                auto& interactPromptTransform = Engine::Registry().get<Transform>(LShiftPrompt);
                interactPromptTransform.position.x = playerTransform.position.x;
                interactPromptTransform.position.y = playerTransform.position.y - 130;
            }

            if (Engine::Registry().has<Transform>(WJPrompt))
            {
                auto& interactPromptTransform = Engine::Registry().get<Transform>(WJPrompt);
                interactPromptTransform.position.x = playerTransform.position.x;
                interactPromptTransform.position.y = playerTransform.position.y - 130;
            }
        }
        else 
        {
            //hide all the displayed prompts
            if (Engine::Registry().has<Transform>(PressEPrompt))
            {
                auto& interactPromptTransform = Engine::Registry().get<Transform>(PressEPrompt);
                interactPromptTransform.position.z = -1;
            }

            if (Engine::Registry().has<Transform>(LShiftPrompt))
            {
                auto& interactPromptTransform = Engine::Registry().get<Transform>(LShiftPrompt);
                interactPromptTransform.position.z = -1;
            }

            if (Engine::Registry().has<Transform>(WJPrompt))
            {
                auto& interactPromptTransform = Engine::Registry().get<Transform>(WJPrompt);
                interactPromptTransform.position.z = -1;
            }
        }
    }
}


void IntearactionSystem::OnKeyboardEvent(KeyboardEvent kEvent_)
{
    if (interactionInputEnabled && kEvent_.key == EDaggerKeyboard::KeyE && kEvent_.action == EDaggerInputState::Pressed)
    {
        //std::cout << "Pressed E\n";
        UnlockAbility(nearestLoreStone.second);
    }
}

void IntearactionSystem::UnlockAbility(int abilityId)
{
    auto&& view = Engine::Registry().view<AtonementController::AtonementCharacter, InputReceiver>();

    for (const auto& entity : view)
    {
        auto&& input = view.get<InputReceiver>(entity);

        // abilityId = 1  -> DASH
        if (abilityId == 1) 
        {
            input.contexts.pop_back();
            input.contexts.push_back("ATON1");

            //display 
            if (Engine::Registry().has<Transform>(LShiftPrompt))
            {
                auto& interactPromptTransform = Engine::Registry().get<Transform>(LShiftPrompt);
                interactPromptTransform.position.z = 10;
            }
        }
        // abilityId = 2  -> JUMP
        else if (abilityId == 2) 
        {
            input.contexts.pop_back();
            input.contexts.push_back("ATON2");

            if (Engine::Registry().has<Transform>(WJPrompt))
            {
                auto& interactPromptTransform = Engine::Registry().get<Transform>(WJPrompt);
                interactPromptTransform.position.z = 10;
            }
        }
        // abilityId = 3  -> THE END
        else if (abilityId == 3) 
        {
            Engine::ToggleSystemsPause(true);
            AtonementPauseSystem::setPausedESC(true);
            AtonementEndScreen::RemoveFromScreenToggle();
            
        }

        //std::cout << "ability unlocked: id = " << abilityId << std::endl;
    }
}