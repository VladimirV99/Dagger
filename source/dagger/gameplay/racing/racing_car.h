#pragma once

#include "core/core.h"
#include "core/system.h"

using namespace dagger;

namespace racing_game
{
	struct RacingCar
	{
		Float32 speed;
	};

	class RacingCarSystem : public System
	{
	public:
		inline String SystemName() const override
		{
			return "Racing Cars System";
		}

		void Run() override;
	};
} // namespace racing_game
