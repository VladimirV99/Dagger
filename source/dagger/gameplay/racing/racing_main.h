#pragma once

#include "core/core.h"
#include "core/engine.h"
#include "core/game.h"
#include "core/system.h"

using namespace dagger;

namespace racing_game
{
	void SetupWorld();

	class RacingGame : public Game
	{
		inline String GetIniFile() const override
		{
			return "racing.ini";
		};

		void GameplaySystemsSetup() override;
		void WorldSetup() override;
	};
} // namespace racing_game
