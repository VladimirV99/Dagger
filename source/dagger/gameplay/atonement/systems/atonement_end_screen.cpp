#pragma once
#include <imgui/imgui.h>
#include "atonement_end_screen.h"
#include "core/core.h"
#include "core/engine.h"
#include "gameplay/atonement/atonement_main.h"
#include "core/graphics/sprite.h"
#include "core/game/transforms.h"
#include "atonement_pause_system.h"
#include "atonement_start_menu.h"
#include <iostream>


using namespace dagger;
using namespace atonement;

bool AtonementEndScreen::onScreen = false;

String AtonementEndScreen::SystemName(){return "AtonementEndSCreen";}

void AtonementEndScreen::SpinUp(){
    Engine::Dispatcher().sink<KeyboardEvent>().template connect<&AtonementEndScreen::OnKeyboardEvent>(this);
    AtonementEndScreen::BuildMenu();
    AtonementEndScreen::onScreen = false;
}

void AtonementEndScreen::Run(){
}

void AtonementEndScreen::BuildMenu(){

     auto* camera = Engine::GetDefaultResource<Camera>();
     camera->Update();
     auto camPosition = (Vector2)camera->position;

    Engine::ToggleSystemsPause(true);
    AtonementPauseSystem::setPausedESC(true);
    auto& reg = Engine::Registry();

    auto entity = reg.create();
    auto& sprite = reg.emplace<Sprite>(entity);
    AssignSprite(sprite, "MenuTextures:EndScreen");

    auto& transform4 = reg.emplace<Transform>(entity);

        transform4.position.x = camPosition.x - 100;
        transform4.position.y = camPosition.y ;
        transform4.position.z = -3.f;

    auto& onscreen4 = reg.emplace<OnScreenToggleEnd>(entity);
}

void AtonementEndScreen::RemoveFromScreenToggle(){
     auto* camera = Engine::GetDefaultResource<Camera>();
     camera->Update();
     auto camPosition = (Vector2)camera->position;

    AtonementPauseSystem::setPausedESC(true);
    auto view = Engine::Registry().view<Transform, OnScreenToggleEnd>();
    for (auto entity : view){
        auto& t = view.get<Transform>(entity);
        auto& ctrl = view.get<OnScreenToggleEnd>(entity); 
        t.position.x = camPosition.x;
        t.position.y = camPosition.y;
        t.position.z = t.position.z * -1;

        if (t.position.z < 0)
        {
            AtonementEndScreen::onScreen = false;
        }
        else
        {
            AtonementPauseSystem::setPausedESC(true);
            AtonementEndScreen::onScreen = true;
             
        }
    }
}

void AtonementEndScreen::WindDown(){
Engine::Dispatcher().sink<KeyboardEvent>().template disconnect<&AtonementEndScreen::OnKeyboardEvent>(this);
}

void AtonementEndScreen::OnKeyboardEvent(KeyboardEvent kEvent_){
     Engine::Registry().view<OnScreenToggleEnd>().each([&](OnScreenToggleEnd& ctrl_)
        {
            if (AtonementEndScreen::onScreen == true){
                if (kEvent_.key == ctrl_.enterKey && kEvent_.action == EDaggerInputState::Pressed)
                {
                   
                    Engine::ToggleSystemsPause(true);
                    atonement::AtonementGame::RestartGame();
                    AtonementStartMenu::DestroyMenu();
                    //AtonementStartMenu::RemoveFromScreenToggle();
                    AtonementStartMenu::BuildMenu();
                    RemoveFromScreenToggle();
                    AtonementPauseSystem::setPausedESC(true);
                }
                
                
            }
        }
        );
        
}