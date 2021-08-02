#pragma once

#include "core/core.h"

enum struct ETextDirection
{
	RIGHT,
	DOWN
};

enum struct ETextAlignment
{
	START,
	MIDDLE,
	END
};

struct Text
{
	String font;
	Vector4 color {1.0f, 1.0f, 1.0f, 1.0f};
	ETextDirection direction {ETextDirection::RIGHT};
	ETextAlignment alignment {ETextAlignment::MIDDLE};
	Vector2 scale {1.0f, 1.0f};
	Float32 spacing {1.0f};
	Sequence<Entity> entities;

	Vector3 position;

	void Set(String font_, String message_, Vector3 pos_ = {0, 0, 0}, Bool ui_ = true);
	void Clear();
};