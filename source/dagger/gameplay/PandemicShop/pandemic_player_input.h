#pragma once

#include "core/system.h"
#include "core/core.h"

using namespace dagger;

namespace pandemic_shop
{
    struct GameStats
    {
        Float32 BorderUp;
        Float32 BorderDown;
        Float32 BorderRight;
        Float32 BorderLeft;
        Float32 PlayerSpeed;

        inline void SetupPlayerBorders(Float32 borderUp_, Float32 borderDown_,
            Float32 borderRight_, Float32 borderLeft_)
        {
            BorderUp = borderUp_;
            BorderDown = borderDown_;
            BorderRight = borderRight_;
            BorderLeft = borderLeft_;
        }
    };
}