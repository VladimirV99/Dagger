#pragma once

#include "core/system.h"
#include "core/core.h"
#include "core/engine.h"

using namespace dagger;

struct AtonementPauseSystem : public System 
{
public:
    static bool pausedESC;
    AtonementPauseSystem();

    inline String SystemName() { return "Atonement Pause System "; }

    void SpinUp();
    void WindDown();
    void OnKeyboardEvent(KeyboardEvent kEvent_);
    static void setPausedESC(bool value);
};