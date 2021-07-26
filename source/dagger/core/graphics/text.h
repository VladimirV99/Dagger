#pragma once

#include "core/core.h"

enum struct ETextAlignment
{
	LEFT,
	CENTER,
	RIGHT
};

struct Text
{
	String font;
	ETextAlignment alignment {ETextAlignment::CENTER};
	Vector2 scale {1.0f, 1.0f};
	Float32 spacing {1.0f};
	Sequence<Entity> entities;

	Vector3 position;

	void Set(String font_, String message_, Vector3 pos_ = {0, 0, 0}, Bool ui_ = true);
};