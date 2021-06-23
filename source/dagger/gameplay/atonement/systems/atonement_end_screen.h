#pragma once
#include "core/system.h"
#include "core/core.h"
#include "tools/toolmenu.h"

using namespace dagger;

namespace atonement {

    struct OnScreenToggleEnd
    {
        EDaggerKeyboard enterKey = EDaggerKeyboard::KeyEnter;
        Vector4 input{ 0, 0, 0, 0};
    };


    struct AtonementEndScreen : public System {
        private:
         static bool onScreen;
         void OnKeyboardEvent(KeyboardEvent kEvent_);
        public:
         void SpinUp() override;
         void Run() override;
         void WindDown() override;
         static void BuildMenu();
         static void RemoveFromScreenToggle();
         String SystemName();
    };
};