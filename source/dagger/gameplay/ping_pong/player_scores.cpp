#include "player_scores.h"

#include "core/engine.h"
#include "core/game/transforms.h"
#include "core/graphics/text.h"

#include "gameplay/common/simple_collisions.h"
#include "gameplay/ping_pong/pingpong_ball.h"
#include "gameplay/ping_pong/ping_pong_main.h"

using namespace ping_pong;

int PlayerScoresSystem::s_FieldWidth = 20;
int PlayerScoresSystem::s_FieldHeight = 10;
Float32 PlayerScoresSystem::s_TileSize = 1.f;

void PlayerScoresSystem::Run()
{
    auto view = Engine::Registry().view<PingPongBall, Transform>();

    int ballOnField = 0;

    for (auto entity : view)
    {
        auto &ball = view.get<PingPongBall>(entity);
        auto &t = view.get<Transform>(entity);

        if (ball.reachedGoal && !ball.processed)
        {
            ball.speed = { 0,0,0 };

            if (ball.playerOneScored)
            {
                m_goalsPlayerOne++;
                t.position = { (-(s_FieldWidth + 2) / 2.f - ((m_goalsPlayerOne - 1) / 10)) * s_TileSize, -s_TileSize * (-2 + (m_goalsPlayerOne - 1) % 10), 0 };
            }
            else
            {
                m_goalsPlayerTwo++;
                t.position = { ((s_FieldWidth + 3) / 2.f + ((m_goalsPlayerTwo - 1) / 10)) * s_TileSize, -s_TileSize * (-2 + (m_goalsPlayerTwo - 1) % 10), 0 };
            }

            ball.processed = true;
        }

        if (!ball.reachedGoal)
        {
            ballOnField++;
        }
    }

    auto scoreView = Engine::Registry().view<PlayerScore, Text>();
    for (auto score : scoreView)
    {
        auto& ps = scoreView.get<PlayerScore>(score);
        auto& txt = scoreView.get<Text>(score);

        if (ps.isLeft)
        {
            if (ps.score != m_goalsPlayerOne) {
                ps.score = m_goalsPlayerOne;
                txt.Set("pixel-font", std::to_string(m_goalsPlayerOne), { -(s_FieldWidth + 3) * s_TileSize / 2, 80, 0 });
            }
        }
        else
        {
            if (ps.score != m_goalsPlayerTwo) {
                ps.score = m_goalsPlayerTwo;
                txt.Set("pixel-font", std::to_string(m_goalsPlayerTwo), { (s_FieldWidth + 5) * s_TileSize / 2, 80, 0 });
            }
        }
    }

    if (ballOnField == 0)
    {
        // Done without if for better performance
        CreateRandomPingPongBall(
            s_TileSize,
            s_FieldHeight
        );
    }
}
