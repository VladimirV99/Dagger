#pragma once

#include "core/core.h"
#include "core/engine.h"
#include "core/game.h"

using namespace dagger;

namespace multiplayer
{
    class MultiplayerGame : public Game
	{
		inline String GetIniFile() const override
		{
			return "multiplayer.ini";
		};

		void GameplaySystemsSetup() override;
		void WorldSetup() override;
	};
} // namespace multiplayer