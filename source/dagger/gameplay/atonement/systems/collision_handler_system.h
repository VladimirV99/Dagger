#pragma once

#include "core/system.h"
#include "core/core.h"

using namespace dagger;

namespace atonement {

    struct PlayerDeathEvent {};

    class CollisionHandlerSystem : public System
    {
    public:
        inline String SystemName() { return "Collision Handler System"; }

        void Run() override;
    };
}
