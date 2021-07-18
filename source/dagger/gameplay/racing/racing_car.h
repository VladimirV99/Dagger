#pragma once

#include "core/system.h"
#include "core/core.h"

using namespace dagger;

namespace racing_game
{
    struct RacingCar
    {
        Float32 speed;
    };

    class RacingCarSystem
        : public System
    {
    public:
        inline String SystemName() const override { return "Racing Cars System"; }

        void Run() override;
    };
}
