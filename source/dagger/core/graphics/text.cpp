#include "text.h"

#include "core/core.h"
#include "core/engine.h"
#include "core/graphics/sprite.h"

using namespace dagger;

void Text::Set(String font_, String message_, Vector3 pos_, Bool ui_)
{
	font = font_;
	position = pos_;

	auto& registry = Engine::Registry();

	assert(Engine::Res<Texture>().contains(fmt::format("spritesheets:{}", font_)));

	Clear();

	SInt32 positionX = position.x;
	auto& sheets = Engine::Res<SpriteFrame>();

	Map<UInt32, SpriteFrame*> cache;
	UInt32 fullStringWidth = 0;
	for (char letter : message_)
	{
		cache[letter] = sheets[fmt::format("spritesheets:{}:{}", font, (int)letter)];
		fullStringWidth += cache[letter]->frame.size.x * scale.x * spacing;
	}

	Float32 xOffsetDueToAlign = 0.0f;
	if (alignment == ETextAlignment::CENTER)
		xOffsetDueToAlign = (Float32)fullStringWidth / 2.0f;
	else if (alignment == ETextAlignment::RIGHT)
		xOffsetDueToAlign = (Float32)fullStringWidth;

	for (char letter : message_)
	{
		auto* spritesheet = cache[letter];
		auto entity = registry.create();
		auto& sprite = registry.emplace<Sprite>(entity);

		if (ui_)
			sprite.UseAsUI();
		sprite.position = {positionX + (spritesheet->frame.size.x * scale.x * spacing / 2.0f) - xOffsetDueToAlign, position.y, position.z};
		sprite.scale = scale;
		AssignSprite(sprite, spritesheet);

		positionX += (int)(spritesheet->frame.size.x * scale.x * spacing);
		entities.push_back(entity);
	}
}

void Text::Clear()
{
	auto& registry = Engine::Registry();

	if (!entities.empty())
	{
		registry.destroy(entities.begin(), entities.end());
		entities.clear();
	}
}