#pragma once

#include "core/core.h"
#include "core/engine.h"
#include <optional>

using namespace dagger;

namespace pandemic {

	class Level
	{
	public:
		static constexpr float TILE_WIDTH = 20.0f;
		static constexpr float TILE_HEIGHT = 20.0f;

		static unsigned LEVEL_WIDTH;
		static unsigned LEVEL_HEIGHT;

		static void Load(String name);
		
		static Vector2 getPosition(int x, int y);
		inline static float getPositionX(int x);
		inline static float getPositionY(int y);
	};

}