#include "camera_focus.h"

#include "core/core.h"
#include "core/engine.h"
#include "core/graphics/sprite.h"
#include "core/graphics/window.h"

using namespace dagger;

void CameraFollowSystem::Run()
{
    auto* camera = Engine::GetDefaultResource<Camera>();
    auto camPosition = (Vector2)camera->position;

    Vector2 center{};
    UInt32 count{ 0 };

    Engine::Registry().view<CameraFollowFocus, Sprite>().each([&](const CameraFollowFocus& focus_, const Sprite& sprite_)
        {
            for (UInt32 i = 0; i < focus_.weight; i++)
            {   
                //+ Vector2{0, 200} je da nebi bio igrac bas u centru nego malo ispod
                center += (Vector2)sprite_.position + Vector2{0, 200};
            }
            count += focus_.weight;
        });

    if (count > 0)
    {
        center /= count;
        camera->position = Vector3{ glm::mix((Vector2)camera->position, center, 0.5f), 0.0f };
    }
}
