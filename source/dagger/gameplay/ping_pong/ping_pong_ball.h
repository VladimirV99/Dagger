#pragma once

#include "core/system.h"
#include "core/core.h"
#include "core/game/transforms.h"
#include "gameplay/common/simple_collisions.h"

using namespace dagger;

namespace ping_pong
{
    struct PingPongBall
    {
        Vector3 speed{ 0, 0, 0 };

        bool reachedGoal{ false };
        bool playerOneScored{ true };
        bool processed{ false };
    };

    struct PingPongWall
    {
        bool isLeft{ true };
    };

    class PingPongBallSystem
        : public System
    {
        void ResolveCollision(Transform& t_, SimpleCollision& col_, PingPongBall& ball_,
            const Transform& otherTransform_, const SimpleCollision& otherCollision_);

    public:

        inline String SystemName() const override { return "PingPong Ball System"; }

        void Run() override;
    };
}