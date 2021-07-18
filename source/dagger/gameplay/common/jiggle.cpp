
#include "jiggle.h"

#include "core/engine.h"
#include "core/graphics/sprite.h"

#include <algorithm>
#include <execution>

using namespace dagger;

void JiggleSystem::Run()
{
	Engine::Registry().view<Sprite>().each(
		[&](Sprite& sprite_)
		{
			sprite_.position.x += ((rand() % 3) - 1) * 0.001f;
			sprite_.position.y += ((rand() % 3) - 1) * 0.001f;

			sprite_.color.r += ((rand() % 3) - 1) * 0.01f;
			sprite_.color.g += ((rand() % 3) - 1) * 0.01f;
			sprite_.color.b += ((rand() % 3) - 1) * 0.01f;
		});
}
