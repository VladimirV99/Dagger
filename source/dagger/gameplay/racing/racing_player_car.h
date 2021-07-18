#pragma once

#include "core/core.h"
#include "core/system.h"

using namespace dagger;

namespace racing_game
{
	struct ControllerMapping
	{
		EDaggerKeyboard leftKey = EDaggerKeyboard::KeyA;
		EDaggerKeyboard rightKey = EDaggerKeyboard::KeyD;

		Vector2 input {0, 0};
	};

	struct RacingPlayerCar
	{
		Float32 horzSpeed;
	};

	class RacingPlayerInputSystem : public System
	{
	public:
		inline String SystemName() const override
		{
			return "Racing Player Input System";
		}

		void SpinUp() override;
		void WindDown() override;
		void Run() override;

	private:
		void OnKeyboardEvent(KeyboardEvent kEvent_);
	};
} // namespace racing_game
