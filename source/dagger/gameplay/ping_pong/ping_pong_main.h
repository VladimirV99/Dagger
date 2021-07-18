#pragma once

#include "core/core.h"
#include "core/engine.h"
#include "core/game.h"
#include "core/system.h"

using namespace dagger;

namespace ping_pong
{
	void CreateRandomPingPongBall(float tileSize_, int fieldHeight_);
	void CreatePingPongBall(float tileSize_, ColorRGBA color_, Vector3 speed_, Vector3 pos_);
	void SetupWorld();

	class PingPongGame : public Game
	{
		inline String GetIniFile() const override
		{
			return "pingpong.ini";
		};

		void CoreSystemsSetup() override;
		void GameplaySystemsSetup() override;
		void WorldSetup() override;
	};
} // namespace ping_pong
