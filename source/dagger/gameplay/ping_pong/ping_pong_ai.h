#pragma once
#include "core/system.h"

#include "core/game/transforms.h"
#include "ping_pong_ball.h"

using namespace dagger;

namespace ping_pong
{
	enum class EPlayerSide {
		LEFT,
		RIGHT
	};

	struct AI {
		EPlayerSide side;
	};

	class PingPongAISystem
		: public System
	{
		static Float32 s_BoarderUp;
		static Float32 s_BoarderDown;
		static Float32 s_PlayerSize;

		Bool ShouldConsiderBall(const Transform& ballTransform_, const PingPongBall& ball_, const Transform& playerTransform_, const AI& playerAI_);

	public:
		static Float32 s_AIPlayerSpeed;

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