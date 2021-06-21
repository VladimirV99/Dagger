#include "gameplay/PandemicShop/karen_controller.h"

#include "core/core.h"
#include "core/engine.h"
#include "core/graphics/animation.h"
#include "core/graphics/shaders.h"
#include "core/graphics/sprite.h"
#include "core/graphics/window.h"
#include "core/input/inputs.h"

using namespace pandemic;

void KarenControllerSystem::Run() {
    Engine::Registry().view<KarenControllerFSM::StateComponent>().each(
        [&](KarenControllerFSM::StateComponent& state_) {
            characterFSM.Run(state_);
        });
}
