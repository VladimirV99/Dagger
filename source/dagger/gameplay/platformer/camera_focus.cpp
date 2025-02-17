#include "camera_focus.h"

#include "core/core.h"
#include "core/engine.h"
#include "core/graphics/camera.h"
#include "core/graphics/sprite.h"

using namespace dagger;
using namespace platformer;

void CameraFollowSystem::Run()
{
	auto* camera = Engine::GetDefaultResource<Camera>();

	Vector2 center {};
	UInt32 count {0};

	Engine::Registry().view<CameraFollowFocus, Sprite>().each(
		[&](const CameraFollowFocus& focus_, const Sprite& sprite_)
		{
			for (UInt32 i = 0; i < focus_.weight; i++)
			{
				center += (Vector2)sprite_.position;
			}
			count += focus_.weight;
		});

	if (count > 0)
	{
		center /= count;
		camera->position = Vector3 {glm::mix((Vector2)camera->position, center, 0.5f), 0.0f};
	}
}
