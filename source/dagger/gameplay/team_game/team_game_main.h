#pragma once

#include "core/core.h"
#include "core/engine.h"
#include "core/game.h"
#include "core/system.h"

using namespace dagger;

namespace team_game
{
	void SetupWorld();

	class TeamGame : public Game
	{
		inline String GetIniFile() const override
		{
			return "teamgame.ini";
		};

		void GameplaySystemsSetup() override;
		void WorldSetup() override;
	};
} // namespace team_game
