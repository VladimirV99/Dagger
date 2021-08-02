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

	SInt32 currentPosition;
	if (direction == ETextDirection::RIGHT)
		currentPosition = position.x;
	else if (direction == ETextDirection::DOWN)
		currentPosition = position.y;

	auto& sheets = Engine::Res<SpriteFrame>();

	Map<UInt32, SpriteFrame*> cache;
	UInt32 fullStringWidth = 0;
	for (char letter : message_)
	{
		cache[letter] = sheets[fmt::format("spritesheets:{}:{}", font, (int)letter)];
		if (direction == ETextDirection::RIGHT)
			fullStringWidth += cache[letter]->frame.size.x * scale.x * spacing;
		else if (direction == ETextDirection::DOWN)
			fullStringWidth += cache[letter]->frame.size.y * scale.y * spacing;
	}

	Float32 alignOffset = 0.0f;
	if (alignment == ETextAlignment::MIDDLE)
		alignOffset = (Float32)fullStringWidth / 2.0f;
	else if (alignment == ETextAlignment::END)
		alignOffset = (Float32)fullStringWidth;

	for (char letter : message_)
	{
		auto* spritesheet = cache[letter];
		auto entity = registry.create();
		auto& sprite = registry.emplace<Sprite>(entity);

		if (ui_)
			sprite.UseAsUI();

		if (direction == ETextDirection::RIGHT)
			sprite.position = {
				currentPosition + (spritesheet->frame.size.x * scale.x * spacing / 2.0f) - alignOffset, position.y,
				position.z};
		else if (direction == ETextDirection::DOWN)
			sprite.position = {
				position.x, currentPosition - (spritesheet->frame.size.x * scale.x * spacing / 2.0f) + alignOffset,
				position.z};

		sprite.color = color;
		sprite.scale = scale;
		AssignSprite(sprite, spritesheet);

		if (direction == ETextDirection::RIGHT)
			currentPosition += (int)(spritesheet->frame.size.x * scale.x * spacing);
		else if (direction == ETextDirection::DOWN)
			currentPosition -= (int)(spritesheet->frame.size.y * scale.y * spacing);

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