#pragma once

#include "core/core.h"
#include "core/engine.h"
#include "core/game.h"
#include "core/system.h"

using namespace dagger;

namespace tiles_example
{
	class TilesExampleMain : public Game
	{
		inline String GetIniFile() const override
		{
			return "tiles_example.ini";
		};

		void GameplaySystemsSetup() override;
		void WorldSetup() override;
	};
} // namespace tiles_example
