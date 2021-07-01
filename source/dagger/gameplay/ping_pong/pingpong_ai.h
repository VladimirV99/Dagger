#pragma once
#include "core/system.h"

using namespace dagger;

namespace ping_pong
{
	enum class PlayerSide {
		LEFT,
		RIGHT
	};

	struct AI {
		PlayerSide side;
	};

	class PingPongAISystem
		: public System
	{
		static Float32 s_BoarderUp;
		static Float32 s_BoarderDown;
		static Float32 s_PlayerSize;

	public:
		static Float32 s_PlayerSpeed;

	public:
		inline String SystemName() override {
			return "AI System";
		}

		void Run() override;

		static void SetupPlayerBoarders(Float32 boarderUp_, Float32 boarderDown_, Float32 playerSize_)
		{
			s_BoarderUp = boarderUp_;
			s_BoarderDown = boarderDown_;
			s_PlayerSize = playerSize_;
		}
	};
}