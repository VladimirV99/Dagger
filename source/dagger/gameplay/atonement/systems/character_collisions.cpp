#include "gameplay/atonement/systems/character_collisions.h"
#include "gameplay/common/simple_collisions.h"

#include "core/engine.h"
#include "core/game/transforms.h"

#include <iostream>

using namespace dagger;
using namespace atonement;

void CharacterCollisionsSystem::Run()
{
    auto view = Engine::Registry().view<CharacterCollision, Transform>();

    //samo nas lik ima CharacterCollision
    for (auto& character : view) {

        auto& charCollision = view.get<CharacterCollision>(character);
        auto& charTransform = view.get<Transform>(character);

        //resetujemo sve kolizije koje su postojale u proslom frejmu
        charCollision.collidedLeft = false;
        charCollision.collidedRight = false;
        charCollision.collidedUp = false;
        charCollision.collidedDown = false;

        charCollision.collidedWithLeft = entt::null;
        charCollision.collidedWithRight = entt::null;
        charCollision.collidedWithUp = entt::null;
        charCollision.collidedWithDown = entt::null;

        //svi ostali entiteti sa kolizijom
        auto simpleView = Engine::Registry().view<SimpleCollision, Transform>();
        for (auto& entity : simpleView) {

            auto& otherCollision = simpleView.get<SimpleCollision>(entity);
            auto& otherTransform = simpleView.get<Transform>(entity);

            CollisionSide collisionDetection = charCollision.IsCollided(charTransform.position, otherTransform.position, otherCollision);

            switch (collisionDetection) {
            case CollisionSide::Left:
                charCollision.collidedLeft = true;
                charCollision.collidedWithLeft = entity;
                break;
            case CollisionSide::Right:
                charCollision.collidedRight = true;
                charCollision.collidedWithRight = entity;
                break;
            case CollisionSide::Up:
                charCollision.collidedUp = true;
                charCollision.collidedWithUp = entity;
                break;
            case CollisionSide::Down:
                charCollision.collidedDown = true;
                charCollision.collidedWithDown = entity;
                break;
            default: break;
            }
        }

    }
}

CollisionSide CharacterCollision::IsCollided(const Vector3& pos_, const Vector3& posOther_, const SimpleCollision& colOther_)
{
    Vector2 p(pos_.x, pos_.y);
    Vector2 p2(posOther_.x, posOther_.y);

    Float32 horizontalOverlap = 0;
    Float32 verticalOverlap = 0;

    if (std::abs(p2.x - p.x) < (size.x + colOther_.size.x) / 2.f &&
        std::abs(p.y - p2.y) < (size.y + colOther_.size.y) / 2.f) {
        
        if (p.x + size.x / 2.f < p2.x + colOther_.size.x / 2.f) {
            horizontalOverlap = (p.x + size.x / 2.f) - (p2.x - colOther_.size.x / 2.f);
        }
        else {
            horizontalOverlap = (p2.x + colOther_.size.x / 2.f) - (p.x - size.x / 2.f);
        }

        if (p.y + size.y / 2.f < p2.y + colOther_.size.y / 2.f) {
            verticalOverlap = (p.y + size.y / 2.f) - (p2.y - colOther_.size.y / 2.f);
        }
        else {
            verticalOverlap = (p2.y + colOther_.size.y / 2.f) - (p.y - size.y / 2.f);
        }

        if (std::abs(horizontalOverlap) < std::abs(verticalOverlap)) {
            if (p.x < p2.x) {
                return CollisionSide::Right;
            }
            else {
                return CollisionSide::Left;
            }
        }
        else {
            if (p.y < p2.y) {
                return CollisionSide::Up;
            }
            else {
                return CollisionSide::Down;
            }
        }
    }
    else {
        return CollisionSide::None;
    }
}

//UNTESTED
Vector3 CharacterCollision::GetCollisionCenter(const Vector3& pos_, const SimpleCollision& other_, const Vector3& posOther_)
{
    Vector3 res;

    Vector2 p(pos_.x + pivot.x * size.x, pos_.y + pivot.y * size.y);
    Vector2 p2(posOther_.x + other_.pivot.x * other_.size.x, posOther_.y + other_.pivot.y * other_.size.y);

    res.x = (p.x + size.x / 2.f + p2.x + other_.size.x) / 2.f;
    res.y = (p.y + size.y / 2.f + p2.y + other_.size.y) / 2.f;
    res.z = 1.f;

    return res;
}
