#include "ping_pong_ai.h"

#include "core/engine.h"
#include "gameplay/common/simple_collisions.h"

using namespace dagger;
using namespace ping_pong;

Float32 PingPongAISystem::s_BoarderDown = -20;
Float32 PingPongAISystem::s_BoarderUp = 20;

Float32 PingPongAISystem::s_PlayerSize = 20;
Float32 PingPongAISystem::s_AIPlayerSpeed = 1.0f;

Bool PingPongAISystem::ShouldConsiderBall(
	const Transform& ballTransform_, const PingPongBall& ball_, const Transform& playerTransform_, const AI& playerAI_)
{
	if (playerAI_.side == EPlayerSide::LEFT)
	{
		return !ball_.processed && ball_.speed.x < 0 && ballTransform_.position.x < 0 &&
			   ballTransform_.position.x > playerTransform_.position.x + 10;
	}
	else if (playerAI_.side == EPlayerSide::RIGHT)
	{
		return !ball_.processed && ball_.speed.x > 0 && ballTransform_.position.x > 0 &&
			   ballTransform_.position.x < playerTransform_.position.x - 10;
	}
	return false;
}

void PingPongAISystem::Run()
{
	auto view = Engine::Registry().view<Transform, AI>();
	auto ballView = Engine::Registry().view<PingPongBall, Transform, SimpleCollision>();

	for (auto entity : view)
	{
		auto& t = view.get<Transform>(entity);
		auto& ai = view.get<AI>(entity);

		std::vector<std::pair<float, float>> balls;

		for (auto ball : ballView)
		{
			auto& b = ballView.get<PingPongBall>(ball);
			auto& bt = ballView.get<Transform>(ball);

			if (ShouldConsiderBall(bt, b, t, ai))
				balls.emplace_back(bt.position.x / -b.speed.x, bt.position.y + 0.01f * b.speed.y);
		}

		float goal = 0;
		if (!balls.empty())
		{
			std::sort(balls.begin(), balls.end());
			float percentage = 0.9;
			float totalPercentage = 0;
			for (auto& pair : balls)
			{
				goal += percentage * pair.second;
				totalPercentage += percentage;
				percentage *= 0.1;
			}
			goal /= totalPercentage;
		}

		if (fabs(t.position.y - goal) > (s_PlayerSize * 0.25f))
			t.position.y += (t.position.y < goal ? 1.0f : -1.0f) * s_AIPlayerSpeed * Engine::DeltaTime();

		if (t.position.y > s_BoarderUp)
			t.position.y = s_BoarderUp;
		if (t.position.y < s_BoarderDown)
			t.position.y = s_BoarderDown;
	}
}