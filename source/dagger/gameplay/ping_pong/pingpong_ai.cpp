#include "pingpong_ai.h"

#include "core/engine.h"
#include "core/game/transforms.h"
#include "pingpong_ball.h"
#include "gameplay/common/simple_collisions.h"

using namespace dagger;
using namespace ping_pong;

Float32 PingPongAISystem::s_BoarderDown = -20;
Float32 PingPongAISystem::s_BoarderUp = 20;

Float32 PingPongAISystem::s_PlayerSize = 20;
Float32 PingPongAISystem::s_PlayerSpeed = 1.0f;

void PingPongAISystem::Run()
{
    auto view = Engine::Registry().view<Transform, AI>();
    auto ballView = Engine::Registry().view<PingPongBall, Transform, SimpleCollision>();

    for (auto entity : view)
    {
        auto& t = view.get<Transform>(entity);
        auto& ai = view.get<AI>(entity);

        std::vector<std::pair<float, float>> balls;

        if (ai.side == PlayerSide::LEFT)
        {
            for (auto ball : ballView)
            {
                auto& b = ballView.get<PingPongBall>(ball);
                auto& bt = ballView.get<Transform>(ball);

                if (!b.processed && b.speed.x < 0 && bt.position.x < 0 && bt.position.x > t.position.x + 10)
                {
                    balls.push_back({ bt.position.x / -b.speed.x, bt.position.y + 0.01f * b.speed.y });
                }
            }
        }
        else if (ai.side == PlayerSide::RIGHT)
        {
            for (auto ball : ballView)
            {
                auto& b = ballView.get<PingPongBall>(ball);
                auto& bt = ballView.get<Transform>(ball);

                if (!b.processed && b.speed.x > 0 && bt.position.x > 0 && bt.position.x < t.position.x - 10)
                {
                    balls.push_back({ bt.position.x / -b.speed.x, bt.position.y + 0.01f * b.speed.y });
                }
            }
        }

        float goal = 0;
        if (!balls.empty())
        {
            std::sort(balls.begin(), balls.end());
            float percentage = 0.9;
            float total_percentage = 0;
            for (auto& pair : balls)
            {
                goal += percentage * pair.second;
                total_percentage += percentage;
                percentage *= 0.1;
            }
            goal /= total_percentage;
        }

        if (fabs(t.position.y - goal) > (s_PlayerSize * 0.25f))
            t.position.y += (t.position.y < goal ? 1.0f : -1.0f) * s_PlayerSpeed * Engine::DeltaTime();

        if (t.position.y > s_BoarderUp)
        {
            t.position.y = s_BoarderUp;
        }

        if (t.position.y < s_BoarderDown)
        {
            t.position.y = s_BoarderDown;
        }
    }
}