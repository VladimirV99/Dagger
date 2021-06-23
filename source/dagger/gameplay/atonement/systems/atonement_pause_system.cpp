#pragma once

#include "gameplay/atonement/systems/atonement_pause_system.h"
#include "atonement_pause_menu.h"
#include "core/engine.h"
#include "core/system.h"
#include <iostream>

using namespace dagger;
using namespace atonement;

bool AtonementPauseSystem::pausedESC = true;

AtonementPauseSystem::AtonementPauseSystem(){
    canBePaused = false;
};
void AtonementPauseSystem::SpinUp()
{
    pausedESC = true;
    Engine::Dispatcher().sink<KeyboardEvent>().template connect<&AtonementPauseSystem::OnKeyboardEvent>(this);
}

void AtonementPauseSystem::WindDown()
{
    Engine::Dispatcher().sink<KeyboardEvent>().template disconnect<&AtonementPauseSystem::OnKeyboardEvent>(this);
}
void AtonementPauseSystem::OnKeyboardEvent(KeyboardEvent kEvent_)
{
  if (kEvent_.key == EDaggerKeyboard::KeyEscape && kEvent_.action == EDaggerInputState::Released && pausedESC == false)
        {
            if (Engine::s_IsPaused == false){
              Engine::Instance().ToggleSystemsPause(true);
              AtonementPauseMenu::RemoveFromScreenToggle();
            }
            else{
                Engine::Instance().ToggleSystemsPause(false);
                AtonementPauseMenu::RemoveFromScreenToggle();
            }
            
        }
}

void AtonementPauseSystem::setPausedESC(bool value)
{
    pausedESC = value;
}