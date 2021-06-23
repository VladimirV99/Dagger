#pragma once

#include "core/core.h"
#include "core/system.h"
#include "core/engine.h"

#include "gameplay/atonement/atonement_controller.h"

using namespace dagger;

namespace atonement {

    class GroundednessDetectionSystem : public System
    {
    public:
        GroundednessDetectionSystem();

        String SystemName() override {
            return "Groundedness Detection System";
        }

        void Run() override;
    };
}