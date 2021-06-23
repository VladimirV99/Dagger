#pragma once

#include "core/core.h"
#include "core/system.h"
#include "core/graphics/sprite.h"

using namespace dagger;

namespace atonement
{
    struct Parallax
    {
        static Vector2 lastCameraPosition;
        static Float32 strength;
        Entity left;
        Bool assignedLeft = false;
        Entity right;
        Bool assignedRight = false;

        Bool moved = false;

        void assignRight(Entity r) {
            right = r;
            assignedRight = true;
        }

        void assignLeft(Entity l) {
            left = l;
            assignedLeft = true;
        }
    };

    class ParallaxSystem
        : public System
    {
        inline String SystemName() override {
            return "Parallax System";
        };

        void SpinUp() override;

        void Run() override;

    private:
        Entity leftmost;
        Entity rightmost;
    };
}