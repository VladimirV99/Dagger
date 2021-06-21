#pragma once
#include "core/system.h"
#include "core/core.h"
#include "core/game/transforms.h"
#include "gameplay/PandemicShop/ai_system.h"
using namespace dagger;


namespace CollisionType{
    struct Wall{};
    struct Item{};
    struct Char{};
}

struct SimpleCollision
{
    Vector2 size;
    Vector2 pivot {-0.5f, -0.5f};

    bool colided = false;
    entt::entity colidedWith;

    bool IsCollided(const Vector3& pos_, const SimpleCollision& other_, const Vector3& posOther_);

    // return (0,1) if collision happen by y, (1,0) if collision happen by x
    Vector2 GetCollisionSides(const Vector3& pos_, const SimpleCollision& other_, const Vector3& posOther_);

    Vector3 GetCollisionCenter(const Vector3& pos_, const SimpleCollision& other_, const Vector3& posOther_);

};

class SimpleCollisionsSystem : public System
{

public:
    inline String SystemName() { return "Simple Collisions System"; }

    void Run() override;

    void resolveDirection(SimpleCollision &collision, Transform& col_transform, 
                          SimpleCollision& other, Transform &other_transform);
    void resolveItem(SimpleCollision &collision, Transform &col_transform, 
                                            SimpleCollision &other, Transform& other_transform,
                                            AICommand& command);
    void resolveWalls(SimpleCollision &collision, Transform &col_transform, 
                                            SimpleCollision &other, Transform& other_transform,
                                            AICommand& command);
};
