#include "platformer_controller.h"

#include "core/core.h"
#include "core/engine.h"

using namespace platformer;

void PlatformerControllerSystem::Run()
{
    Engine::Registry().view<CharacterControllerFSM::StateComponent>()
        .each([&](CharacterControllerFSM::StateComponent& state_)
            {
                m_CharacterFSM.Run(state_);
            });
}
