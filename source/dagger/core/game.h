#pragma once

#include "core/core.h"

namespace dagger
{
	class Engine;

	struct Game
	{
		virtual ~Game() = default;

		virtual void CoreSystemsSetup();

		virtual String GetIniFile() const = 0;
		virtual void GameplaySystemsSetup() = 0;
		virtual void WorldSetup() = 0;
	};
} // namespace dagger
