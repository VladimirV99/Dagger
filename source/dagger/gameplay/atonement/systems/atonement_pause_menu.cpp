#pragma once
#include <imgui/imgui.h>
#include "atonement_pause_menu.h"
#include "core/core.h"
#include "core/engine.h"
#include "gameplay/atonement/atonement_main.h"
#include "core/graphics/sprite.h"
#include "core/game/transforms.h"
#include "atonement_pause_system.h"
#include "atonement_start_menu.h"


using namespace dagger;
using namespace atonement;

bool AtonementPauseMenu::onScreen = false;

String AtonementPauseMenu::SystemName(){return "AtonementPauseMenu";}

void AtonementPauseMenu::SpinUp(){
    Engine::Dispatcher().sink<KeyboardEvent>().template connect<&AtonementPauseMenu::OnKeyboardEvent>(this);
    AtonementPauseMenu::BuildMenu();
}

void AtonementPauseMenu::Run(){
    Select();
}

void AtonementPauseMenu::BuildMenu(){

     auto* camera = Engine::GetDefaultResource<Camera>();
     camera->Update();
     auto camPosition = (Vector2)camera->position;

    Engine::ToggleSystemsPause(true);
    //AtonementPauseSystem::setPausedESC(true);
    auto& reg = Engine::Registry();
    constexpr int width = 4;
    constexpr int height = 2;
    float tileSize = 300.f;
    constexpr float Space = 0.3f;

    auto entity = reg.create();
    auto& sprite = reg.emplace<Sprite>(entity);
    AssignSprite(sprite, "MenuTextures:selection3");

    sprite.size.x = tileSize;
    sprite.size.y = tileSize/2;

    auto& transform = reg.emplace<Transform>(entity);

        transform.position.x = camPosition.x +(2.0f) * tileSize;
        transform.position.y =  camPosition.y + tileSize/2;
        transform.position.z = -1.f;

    auto& controller = reg.emplace<SelectionMappingPause>(entity);
    auto& onscreen0 = reg.emplace<OnScreenTogglePause>(entity);
    /*
    
    auto entity2 = reg.create();
    auto& sprite2 = reg.emplace<Sprite>(entity2);
    AssignSprite(sprite2, "MenuTextures:start_menu");
        
    auto& transform2 = reg.emplace<Transform>(entity2);

    transform2.position.x = (1.94f + Space - static_cast<float>(width * (1 + Space)) / 2.f) * tileSize;
    transform2.position.y = (1.277f - static_cast<float>(height * (1 + Space)) / 2.f) * tileSize/2;
    transform2.position.z = -2.f;

    auto& onscreen2 = reg.emplace<OnScreenTogglePause>(entity2);

    auto entity3 = reg.create();
    auto& sprite3 = reg.emplace<Sprite>(entity3);
    AssignSprite(sprite3, "MenuTextures:continue");
        
    auto& transform3 = reg.emplace<Transform>(entity3);

    transform3.position.x = (1.94f + Space - static_cast<float>(width * (1 + Space)) / 2.f) * tileSize;
    transform3.position.y = (1.277f + 1 + 1 * Space - static_cast<float>(height * (1 + Space)) / 2.f) * tileSize/2;
    transform3.position.z = -2.f;

    auto& onscreen3 = reg.emplace<OnScreenTogglePause>(entity3);

    auto entity5 = reg.create();
    auto& sprite5 = reg.emplace<Sprite>(entity5);
    AssignSprite(sprite5, "MenuTextures:exit");

    auto& transform5 = reg.emplace<Transform>(entity5);

    transform5.position.x = (1.94f + Space - static_cast<float>(width * (1 + Space)) / 2.f) * tileSize;
    transform5.position.y = (1.277f - 1 - 1 * Space - static_cast<float>(height * (1 + Space)) / 2.f) * tileSize/2;
    transform5.position.z = -2.f;

    auto& onscreen5 = reg.emplace<OnScreenTogglePause>(entity5);
    */

    auto entity4 = reg.create();
    auto& sprite4 = reg.emplace<Sprite>(entity4);
    AssignSprite(sprite4, "MenuTextures:pause_menu_background2");

    auto& transform4 = reg.emplace<Transform>(entity4);

        transform4.position.x = camPosition.x - 100;
        transform4.position.y = camPosition.y - 100;
        transform4.position.z = -3.f;

    auto& onscreen4 = reg.emplace<OnScreenTogglePause>(entity4);
}

void AtonementPauseMenu::Select(){

     auto* camera = Engine::GetDefaultResource<Camera>();
     camera->Update();
     auto camPosition = (Vector2)camera->position;
    
    auto view = Engine::Registry().view<Transform, SelectionMappingPause>();
    for (auto entity : view)
    {
        auto& t = view.get<Transform>(entity);
        auto& ctrl = view.get<SelectionMappingPause>(entity);

        t.position.x = camPosition.x + (-1.0f + ctrl.input.x + ctrl.input.x * 0.3f - static_cast<float>(4 * (1 + 0.3f)) / 2.f) * 30.f;
        t.position.y = camPosition.y + (2.5f + ctrl.input.y + ctrl.input.y * 0.3f - static_cast<float>(4 * (1 + 0.3f)) / 2.f) * 30.f;
    }

}

void AtonementPauseMenu::RemoveFromScreenToggle(){
    //AtonementPauseSystem::setPausedESC(false);
     auto* camera = Engine::GetDefaultResource<Camera>();
     camera->Update();
     auto camPosition = (Vector2)camera->position;

    auto view = Engine::Registry().view<Transform, OnScreenTogglePause>();
    for (auto entity : view){
        auto& t = view.get<Transform>(entity);
        auto& ctrl = view.get<OnScreenTogglePause>(entity); 
        t.position.x = camPosition.x;
        t.position.y = camPosition.y;
        t.position.z = t.position.z  * -1;
        AtonementPauseMenu::onScreen = true;

        if (t.position.z < 0)
        {
            AtonementPauseMenu::onScreen = false;
            AtonementPauseSystem::setPausedESC(false);
        }
        else
        {
             AtonementPauseMenu::onScreen = true;
             AtonementPauseSystem::setPausedESC(true);
        }
    }
}

void AtonementPauseMenu::WindDown(){
Engine::Dispatcher().sink<KeyboardEvent>().template disconnect<&AtonementPauseMenu::OnKeyboardEvent>(this);
}

void AtonementPauseMenu::OnKeyboardEvent(KeyboardEvent kEvent_){
     Engine::Registry().view<SelectionMappingPause>().each([&](SelectionMappingPause& ctrl_)
        {
            if (AtonementPauseMenu::onScreen == true)
            {
                if (kEvent_.key == ctrl_.downKey && kEvent_.action == EDaggerInputState::Pressed && ctrl_.input.y > -5)
                {
                    ctrl_.input.y -= 5;
                }
                else if (kEvent_.key == ctrl_.upKey && kEvent_.action == EDaggerInputState::Pressed && ctrl_.input.y < 5)
                {
                    ctrl_.input.y += 5;
                }
                else if (kEvent_.key == ctrl_.enterKey && kEvent_.action == EDaggerInputState::Pressed && ctrl_.input.y > -5 && ctrl_.input.y <= 0)
                {
                    Engine::ToggleSystemsPause(true);
                    AtonementStartMenu::RemoveFromScreenToggle();
                    AtonementPauseMenu::RemoveFromScreenToggle();
                    AtonementPauseSystem::setPausedESC(true);

                }
                else if (kEvent_.key == ctrl_.enterKey && kEvent_.action == EDaggerInputState::Pressed && ctrl_.input.y > 0)
                {
                    Engine::Instance().ToggleSystemsPause(false);
                    AtonementPauseMenu::RemoveFromScreenToggle(); 
                }
                else if (kEvent_.key == ctrl_.enterKey && kEvent_.action == EDaggerInputState::Pressed && ctrl_.input.y < 5)
                {
                    Engine::Dispatcher().trigger<Exit>();
                }
            }
        }
        );
        
}