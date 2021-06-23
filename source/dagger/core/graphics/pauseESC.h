#pragma once
#include "core/system.h"
#include "core/core.h"
#include "core/engine.h"

using namespace dagger;

struct pauseESC : public System {
    public:
     pauseESC();
     virtual String SystemName();
     void SpinUp();
     void WindDown();
     void OnKeyboardEvent(KeyboardEvent kEvent_);
};