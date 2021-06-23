#include "atonement_controller.h"

using namespace dagger;
using namespace atonement;

AtonementController::AtonementControllerSystem::AtonementControllerSystem() {
    canBePaused = true;
}

void AtonementController::AtonementControllerSystem::Run()
{
    Engine::Registry().view<CharControllerFSM::StateComponent>()
        .each([&](CharControllerFSM::StateComponent& state_)
            {
                characterFSM.Run(state_);
            });
}