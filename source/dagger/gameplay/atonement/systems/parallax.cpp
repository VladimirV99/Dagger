#include "parallax.h"

#include "core/core.h"
#include "core/engine.h"
#include "core/graphics/sprite.h"
#include "core/graphics/window.h"

using namespace dagger;
using namespace atonement;

Vector2 Parallax::lastCameraPosition{ 0, 0 };
Float32 Parallax::strength{ 0 };

/* DO NOT TOUCH THIS RUN()
** DO NOT QUESTION IT
** IT WORKS THROUGH BLACK MAGIC
** Seriously, though, we've had some very weird bugs and had
** to explicitly do a lot of this stuff to force them to go away
** and I'm not sure if it can be refactored or otherwise changed
** without causing them again.
** It probably can, but thread lightly.
**/

void ParallaxSystem::Run()
{
    auto& reg = Engine::Registry();
    auto* camera = Engine::GetDefaultResource<Camera>();
    auto camPosition = (Vector2)camera->position;
    auto delta = (Parallax::lastCameraPosition - camPosition);
    
    Engine::Registry().view<Parallax, Sprite>().each([&](Parallax& parallax_, Sprite& sprite_)
        {
            if(!parallax_.moved){
                sprite_.position.x += delta.x * parallax_.strength;
                sprite_.position.y = camPosition.y;

                if (parallax_.assignedLeft) {
                    auto&& [para, spr] = reg.get<Parallax, Sprite>(parallax_.left);
                    spr.position.x = sprite_.position.x - sprite_.size.x;
                    spr.position.y = camPosition.y;
                    para.moved = true;
                }
                if (parallax_.assignedRight) {
                    auto&& [para, spr] = reg.get<Parallax, Sprite>(parallax_.right);
                    spr.position.x = sprite_.position.x + sprite_.size.x;
                    spr.position.y = camPosition.y;
                    para.moved = true;
                }
            }
        });

    Engine::Registry().view<Parallax>().each([&](Parallax& parallax_)
        {
            parallax_.moved = false;
        });


    Parallax::lastCameraPosition = camPosition;


    auto& rmSprite = reg.get<Sprite>(rightmost);

    if (camPosition.x > rmSprite.position.x) {
        auto entity = reg.create();
        auto& sprite = reg.get_or_emplace<Sprite>(entity);
        auto& parallax = reg.get_or_emplace<Parallax>(entity);
        parallax.lastCameraPosition = camera->position;
        parallax.strength = 0.1;

        AssignSprite(sprite, "MossyBackground:scrolling_bg1");
        sprite.position = { rmSprite.position.x + rmSprite.size.x, -225, 100 };
        sprite.scale = rmSprite.scale;
        sprite.scale.x *= -1;

        parallax.assignLeft(rightmost);
        reg.get<Parallax>(rightmost).assignRight(entity);
        rightmost = entity;
    }

    auto& lmSprite = reg.get<Sprite>(leftmost);

    if (camPosition.x < lmSprite.position.x) {
        auto entity = reg.create();
        auto& sprite = reg.get_or_emplace<Sprite>(entity);
        auto& parallax = reg.get_or_emplace<Parallax>(entity);
        parallax.lastCameraPosition = camera->position;
        parallax.strength = 0.1;

        AssignSprite(sprite, "MossyBackground:scrolling_bg1");
        sprite.position = { lmSprite.position.x - lmSprite.size.x, -225, 100 };
        sprite.scale = lmSprite.scale;
        sprite.scale.x *= -1;

        parallax.assignRight(leftmost);
        reg.get<Parallax>(leftmost).assignLeft(entity);
        leftmost = entity;
    }
}

void ParallaxSystem::SpinUp()
{
    auto* camera = Engine::GetDefaultResource<Camera>();

    auto& reg = Engine::Registry();
    auto entity = reg.create();
    auto& sprite = reg.get_or_emplace<Sprite>(entity);
    auto& parallax = reg.get_or_emplace<Parallax>(entity);
    parallax.lastCameraPosition = camera->position;
    parallax.strength = 0.1;

    AssignSprite(sprite, "MossyBackground:scrolling_bg1");
    sprite.position = { 600, -225, 99 };

    leftmost = entity;
    rightmost = entity;
}