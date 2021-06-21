#pragma once
#include "core/system.h"
#include "core/core.h"
#include "core/engine.h"
#include "core/game/transforms.h"
#include "core/graphics/sprite.h"
#include "gameplay/common/simple_collisions.h"
using namespace dagger;


struct Item {
    bool hidden = false;
    bool pickable = false;    

    Item Create(entt::entity &entity, String sprite_ = "", ColorRGB color_ = {1, 1, 1},
                          Vector2 position_ = {0, 0});
    

};

class Pickable : public System
{
    
public:
    inline String SystemName() { return "Pickable System"; }
    static unsigned inline  picked = 0;
    void Run() override;
};