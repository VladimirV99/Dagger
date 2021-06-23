#pragma once

#include "core/system.h"
#include "core/core.h"

using namespace dagger;

namespace atonement {

    class IntearactionSystem : public System
    {

    public:
        inline String SystemName() { return "Intearaction System"; }

        void SpinUp() override;
        void Run() override;
        void WindDown() override;

    private:
        void OnKeyboardEvent(KeyboardEvent kEvent_);
        void UnlockAbility(int abilityId);

        std::vector<std::pair<Vector3, int>> loreStonePositions;
        std::pair<Vector3, int> nearestLoreStone;
        bool loreStonePositionsLoaded = false;
        bool interactionInputEnabled = false;

        entt::entity PressEPrompt = entt::null;
        entt::entity LShiftPrompt = entt::null;
        entt::entity WJPrompt = entt::null;
    };
}
