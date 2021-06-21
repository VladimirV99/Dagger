#include "gameplay/PandemicShop/pandemic_character_controller.h"

#include "core/core.h"
#include "core/engine.h"
#include "core/graphics/animation.h"
#include "core/graphics/shaders.h"
#include "core/graphics/sprite.h"
#include "core/graphics/window.h"
#include "core/input/inputs.h"

using namespace pandemic;

void PandemicControllerSystem::Run() {
    Engine::Registry().view<PandemicCharacterControllerFSM::StateComponent>().each(
        [&](PandemicCharacterControllerFSM::StateComponent& state_) {
            characterFSM.Run(state_);
        });
}
