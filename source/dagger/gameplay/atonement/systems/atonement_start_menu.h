#pragma once
#include "core/system.h"
#include "core/core.h"
#include "tools/toolmenu.h"

using namespace dagger;

namespace atonement {
    struct SelectionMappingStart
    {
        EDaggerKeyboard upKey = EDaggerKeyboard::KeyUp;
        EDaggerKeyboard downKey = EDaggerKeyboard::KeyDown;
        EDaggerKeyboard enterKey = EDaggerKeyboard::KeyEnter;
        EDaggerKeyboard leftKey = EDaggerKeyboard::KeyLeft;
        Vector4 input{ 0, 0, 0, 0};
    };

    struct OnScreenToggleStart
    {
        EDaggerKeyboard enterKey = EDaggerKeyboard::KeyEnter;
        Vector4 input{ 0, 0, 0, 0};
    };


    struct AtonementStartMenu : public System {
        private:
         static bool onScreen;
         void OnKeyboardEvent(KeyboardEvent kEvent_);
        public:
         void SpinUp() override;
         void Run() override;
         void WindDown() override;
         void Select();
         static void BuildMenu();
         static void RemoveFromScreenToggle();
         static void DestroyMenu();
         String SystemName();
    };
};