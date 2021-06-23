#pragma once

#include "core/core.h"
#include "core/system.h"
#include "core/engine.h"

#include "core/game/transforms.h"
#include "gameplay/common/simple_collisions.h"
#include "gameplay/atonement/systems/character_collisions.h"
#include "gameplay/atonement/atonement_controller.h"
#include "gameplay/atonement/systems/collision_handler_system.h"
#include "gameplay/atonement/components/marker_components.h"
#include <iostream>

using namespace dagger;
using namespace atonement;

void CollisionHandlerSystem::Run() 
{   
        auto& reg = Engine::Registry();
        auto view = reg.view<CharacterCollision>();

        for (auto& entity : view) {

            auto& col = view.get<CharacterCollision>(entity);
            
            /*if (col.collidedLeft) {
                std::cout << "Collided LEFT " << int(entity) << " " << int(col.collidedWithLeft) << std::endl;
            }
            if (col.collidedRight) {
                std::cout << "Collided RIGHT " << int(entity) << " " << int(col.collidedWithRight) << std::endl;
            }
            if (col.collidedUp) {
                std::cout << "Collided UP " << int(entity) << " " << int(col.collidedWithUp) << std::endl;
            }
            if (col.collidedDown) {
                std::cout << "Collided DOWN " << int(entity) << " " << int(col.collidedWithDown) << std::endl;
            }*/


            if ( (reg.valid(col.collidedWithLeft) && reg.has<DeadlyComponent>(col.collidedWithLeft))  ||
                 (reg.valid(col.collidedWithRight) && reg.has<DeadlyComponent>(col.collidedWithRight)) ||
                 (reg.valid(col.collidedWithUp) && reg.has<DeadlyComponent>(col.collidedWithUp)) ||
                 (reg.valid(col.collidedWithDown) && reg.has<DeadlyComponent>(col.collidedWithDown)) )
            {
                Engine::Dispatcher().trigger<PlayerDeathEvent>();
            }

        }
}