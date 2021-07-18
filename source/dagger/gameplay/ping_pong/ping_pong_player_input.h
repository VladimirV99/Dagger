#pragma once

#include "core/core.h"
#include "core/system.h"

using namespace dagger;

namespace ping_pong
{
	struct ControllerMapping
	{
		EDaggerKeyboard upKey;
		EDaggerKeyboard downKey;

		Vector2 input {0, 0};
	};

	class PingPongPlayerInputSystem : public System
	{
		static Float32 s_BoarderUp;
		static Float32 s_BoarderDown;

	public:
		static Float32 s_PlayerSpeed;

		inline String SystemName() const override
		{
			return "PingPong Player Input System";
		}

		void SpinUp() override;
		void WindDown() override;
		void Run() override;

		static void SetupPlayerOneInput(ControllerMapping& controllerMapping_)
		{
			controllerMapping_.upKey = EDaggerKeyboard::KeyW;
			controllerMapping_.downKey = EDaggerKeyboard::KeyS;
		}

		static void SetupPlayerTwoInput(ControllerMapping& controllerMapping_)
		{
			controllerMapping_.upKey = EDaggerKeyboard::KeyUp;
			controllerMapping_.downKey = EDaggerKeyboard::KeyDown;
		}

		static void SetupPlayerBoarders(Float32 boarderUp_, Float32 boarderDown_)
		{
			s_BoarderUp = boarderUp_;
			s_BoarderDown = boarderDown_;
		}

	private:
		void OnKeyboardEvent(KeyboardEvent kEvent_);
	};
} // namespace ping_pong