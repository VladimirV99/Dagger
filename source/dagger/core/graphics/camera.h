#pragma once

#include "core/core.h"
#include "core/engine.h"

using namespace dagger;

enum class ECameraMode
{
	ShowAsMuchAsPossible,
	FixedResolution
};

struct Camera
{
	ECameraMode mode {ECameraMode::FixedResolution};
	Vector3 position {0, 0, 0};
	Vector2 size {800, 600};
	Float32 zoom {1};

	void Update();

	static Vector2 WindowToScreen(Vector2 windowCoord_);
	static Vector2 WindowToWorld(Vector2 windowCoord_);
	static Vector2 WorldToWindow(Vector2 worldCoord_);
};