#include "pauseESC.h"
#include "core/engine.h"
#include "core/system.h"
#include <iostream>
using namespace dagger;

pauseESC::pauseESC(){
    canBePaused = false;
};
void pauseESC::SpinUp()
{
    Engine::Dispatcher().sink<KeyboardEvent>().template connect<&pauseESC::OnKeyboardEvent>(this);
}

void pauseESC::WindDown()
{
    Engine::Dispatcher().sink<KeyboardEvent>().template disconnect<&pauseESC::OnKeyboardEvent>(this);
}
void pauseESC::OnKeyboardEvent(KeyboardEvent kEvent_)
{
  if (kEvent_.key == EDaggerKeyboard::KeyEscape && kEvent_.action == EDaggerInputState::Released)
        {
            std::cout<<"PAUSE";
            Engine::Instance().ToggleSystemsPause(true);
        }
}

String pauseESC::SystemName(){
    return "pauseESC";
};