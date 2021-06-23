#pragma once

#include "core/core.h"
#include "core/system.h"
#include "core/engine.h"

#include "core/game/transforms.h"
#include "gameplay/common/simple_collisions.h"
#include "gameplay/atonement/systems/character_collisions.h"
#include "gameplay/atonement/atonement_controller.h"
#include "gameplay/atonement/systems/checkpoint_system.h"
#include "gameplay/atonement/systems/collision_handler_system.h"
#include <iostream>

using namespace dagger;
using namespace atonement;

void CheckpointSystem::SpinUp() {
    Engine::Dispatcher().sink<PlayerDeathEvent>().connect<&CheckpointSystem::RepositionPlayer>(this);
}

void CheckpointSystem::WindDown() {
    Engine::Dispatcher().sink<PlayerDeathEvent>().disconnect<&CheckpointSystem::RepositionPlayer>(this);
}

void CheckpointSystem::Run()
{
    auto view = Engine::Registry().view<Transform, CharacterCollision>();
    for (auto& entity : view)
    {
        auto& transf = view.get<Transform>(entity);
        for (auto& checkpoint : possibleCheckpoints) 
        {
            if ( std::abs(transf.position.x - checkpoint.x) < 150  && 
                 std::abs(transf.position.y - checkpoint.y) < 150 )
            {
                currentCheckpoint = checkpoint;
                //std::cout << "new checkpoint! " << currentCheckpoint.x << " " << currentCheckpoint.y << std::endl;
            }
        }
    }
}

void CheckpointSystem::RepositionPlayer() 
{
    auto view = Engine::Registry().view<Transform, CharacterCollision>();
    for (auto& entity : view) 
    {
        auto& transf = view.get<Transform>(entity);
        transf.position = currentCheckpoint;
    }
}