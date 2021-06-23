#include "groundedness_detection_system.h"

#include "core/core.h"
#include "core/engine.h"
#include "core/input/inputs.h"
#include "core/graphics/sprite.h"
#include "core/graphics/animation.h"
#include "core/game/transforms.h"
#include "gameplay/atonement/atonement_controller.h"
#include "gameplay/atonement/systems/character_collisions.h"
#include "gameplay/common/cooldown_manager.h"

#include <iostream>

using namespace dagger;
using namespace atonement;

GroundednessDetectionSystem::GroundednessDetectionSystem()
{
    canBePaused = true;
}


void GroundednessDetectionSystem::Run()
{

    auto&& view = Engine::Registry().view<Transform, Sprite, CharacterCollision, AtonementController::AtonementCharacter>();
    for (auto& entity : view) {
        auto&& [transform_, sprite_, collision_, char_] = 
            Engine::Registry().get<Transform, Sprite, CharacterCollision, 
            AtonementController::AtonementCharacter>(entity);

            bool tmp = char_.grounded;

            if (collision_.collidedDown)
            {
                char_.grounded = true;
                char_.dashJumped = false;
            }
            else {
                char_.grounded = false;
            }


            if (char_.grounded != tmp) {
                if (char_.grounded == false) {
                    auto cdManager = Engine::GetDefaultResource<CooldownManager<std::string>>();
                    cdManager->registerCooldown(entity, "wall jump", 0.3);
                }

                //std::cout << "Char pos: " << transform_.position.x << " " << transform_.position.y << std::endl;
                //std::cout << "Grounded: " << (char_.grounded ? "true" : "false") << std::endl;
            }

    }

}