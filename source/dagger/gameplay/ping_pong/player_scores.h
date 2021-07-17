#pragma once
#include "core/system.h"
#include "core/core.h"

#include "gameplay/ping_pong/ping_pong_ball.h"

using namespace dagger;

namespace ping_pong
{
    struct PlayerScore
    {
        bool isLeft;
        int score{ -1 };
    };

    class PlayerScoresSystem : public System
    {
        static int s_FieldWidth;
        static int s_FieldHeight;
        static Float32 s_TileSize;

        int m_GoalsPlayerOne = 0;
        int m_GoalsPlayerTwo = 0;

    public:
        inline String SystemName() override { return "Player Scores System"; }

        void Run() override;


        static Float32 GetTileSize() { return s_TileSize; }
        static void SetFieldSize(int width_, int height_, float tileSize_)
        {
            s_FieldWidth = width_;
            s_FieldHeight = height_;
            s_TileSize = tileSize_;
        }
    };
}