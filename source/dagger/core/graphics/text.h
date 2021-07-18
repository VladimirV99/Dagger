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
	Float32 spacing {1.0f};
	Sequence<Entity> entities;

	Vector3 position;

	void Set(String font_, String message_, Vector3 pos_ = {0, 0, 0}, Bool ui_ = true);
};