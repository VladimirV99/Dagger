#include "ping_pong_main.h"

#include "core/core.h"
#include "core/engine.h"
#include "core/input/inputs.h"
#include "core/graphics/sprite.h"
#include "core/graphics/animation.h"
#include "core/graphics/shaders.h"
#include "core/graphics/window.h"
#include "core/game/transforms.h"
#include "core/graphics/sprite_render.h"
#include "core/graphics/textures.h"
#include "core/graphics/animations.h"
#include "core/graphics/gui.h"
#include "core/graphics/text.h"
#include "tools/diagnostics.h"

#include "gameplay/common/simple_collisions.h"
#include "gameplay/ping_pong/ping_pong_ai.h"
#include "gameplay/ping_pong/ping_pong_ball.h"
#include "gameplay/ping_pong/player_scores.h"
#include "gameplay/ping_pong/ping_pong_player_input.h"
#include "gameplay/ping_pong/ping_pong_tools.h"

using namespace dagger;
using namespace ping_pong;

void ping_pong::CreateRandomPingPongBall(float tileSize_, int fieldHeight_)
{
    // Done without if for better performance
    CreatePingPongBall(
        tileSize_,
        // Random color
        ColorRGBA(rand() % 255 / 255.0f, rand() % 255 / 255.0f, rand() % 255 / 255.0f, 1),
        // Random speed
        // in range [4, 13] in each direction
        // ((rand()%2+2)%3-1) is a -1 or 1 value
        { (rand() % 10 + 4) * ((rand() % 2 + 2) % 3 - 1),(rand() % 10 + 4) * ((rand() % 2 + 2) % 3 - 1),0 },
        // Random position
        // in range [-s_FieldHeight/2+2, s_FieldHeight/2-2]
        { 0,rand() % (fieldHeight_ - 3) + 2 - (fieldHeight_ / 2),0 }
    );
}

void ping_pong::CreatePingPongBall(float tileSize_, ColorRGBA color_, Vector3 speed_, Vector3 pos_)
{
    auto& reg = Engine::Registry();
    auto entity = reg.create();
    auto& sprite = reg.emplace<Sprite>(entity);
    AssignSprite(sprite, "PingPong:ball");
    sprite.size = Vector2(1, 1) * tileSize_;

    sprite.color = color_;

    auto& transform = reg.emplace<Transform>(entity);
    transform.position = pos_ * tileSize_;
    transform.position.z = pos_.z;
    
    auto& ball = reg.emplace<PingPongBall>(entity);
    ball.speed = speed_ * tileSize_;

    auto& col = reg.emplace<SimpleCollision>(entity);
    col.size.x = tileSize_;
    col.size.y = tileSize_;
}

void PingPongGame::CoreSystemsSetup()
{
    auto& engine = Engine::Instance();

    engine.AddSystem<WindowSystem>();
    engine.AddSystem<InputSystem>();
    engine.AddSystem<ShaderSystem>();
    engine.AddSystem<TextureSystem>();
    engine.AddSystem<SpriteRenderSystem>();
    engine.AddPausableSystem<TransformSystem>();
    engine.AddPausableSystem<AnimationSystem>();
#if !defined(NDEBUG)
    engine.AddSystem<DiagnosticSystem>();
    engine.AddSystem<GUISystem>();
    engine.AddSystem<ToolMenuSystem>();
#endif //!defined(NDEBUG)
}

void PingPongGame::GameplaySystemsSetup()
{
    auto& engine = Engine::Instance();

    engine.AddPausableSystem<SimpleCollisionsSystem>();
    engine.AddPausableSystem<PingPongBallSystem>();
    engine.AddPausableSystem<PingPongPlayerInputSystem>();
    engine.AddPausableSystem<PlayerScoresSystem>();
    engine.AddPausableSystem<PingPongAISystem>();
#if defined(DAGGER_DEBUG)
    engine.AddPausableSystem<PingPongTools>();
#endif //defined(DAGGER_DEBUG)
}

void PingPongGame::WorldSetup()
{
    auto* camera = Engine::GetDefaultResource<Camera>();
    camera->mode = ECameraMode::FixedResolution;
    camera->size = { 800, 600 };
    camera->zoom = 1;
    camera->position = { 0, 0, 0 };
    camera->Update();

    SetupWorld();
}

void ping_pong::SetupWorld()
{
    Vector2 scale(1, 1);

    auto& reg = Engine::Registry();

    constexpr int height = 20;
    constexpr int width = 26;
    constexpr float tileSize = 20.0f;

    float zPos = 1.f;

    constexpr float space = 0.1f;
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            auto entity = reg.create();
            auto& sprite = reg.emplace<Sprite>(entity);
            AssignSprite(sprite, "EmptyWhitePixel");
            sprite.size = scale * tileSize;

            if (i % 2 != j % 2)
            {
                sprite.color.r = 0.4f;
                sprite.color.g = 0.4f;
                sprite.color.b = 0.4f;
            }
            else
            {
                sprite.color.r = 0.6f;
                sprite.color.g = 0.6f;
                sprite.color.b = 0.6f;
            }

            if (i == 0 || i == height - 1 || j == 0 || j == width - 1)
            {
                sprite.color.r = 0.0f;
                sprite.color.g = 0.0f;
                sprite.color.b = 0.0f;

                //auto& col = reg.emplace<SimpleCollision>(entity);
                //col.size.x = TileSize;
                //col.size.y = TileSize;
            }

            auto& transform = reg.emplace<Transform>(entity);
            transform.position.x = (0.5f + j + j * space - static_cast<float>(width * (1 + space)) / 2.f) * tileSize;
            transform.position.y = (0.5f + i + i * space - static_cast<float>(height * (1 + space)) / 2.f) * tileSize;
            transform.position.z = zPos;
        }
    }

    zPos -= 1.f;

    // Collisions
    {
        // Up
        {
            auto entity = reg.create();
            auto& col = reg.emplace<SimpleCollision>(entity);
            col.size.x = tileSize * (width - 2)* (1 + space);
            col.size.y = tileSize;

            auto& transform = reg.emplace<Transform>(entity);
            transform.position.x = 0;
            transform.position.y = (0.5f + (height - 1) + (height - 1) * space - static_cast<float>(height * (1 + space)) / 2.f) * tileSize;
            transform.position.z = zPos;
        }

        // Down
        {
            auto entity = reg.create();
            auto& col = reg.emplace<SimpleCollision>(entity);
            col.size.x = tileSize * (width - 2) * (1 + space);
            col.size.y = tileSize;

            auto& transform = reg.emplace<Transform>(entity);
            transform.position.x = 0;
            transform.position.y = (0.5f - static_cast<float>(height * (1 + space)) / 2.f) * tileSize;
            transform.position.z = zPos;
        }

        // Left
        {
            auto entity = reg.create();
            auto& col = reg.emplace<SimpleCollision>(entity);
            col.size.x = tileSize;
            col.size.y = tileSize * (height - 2) * (1 + space);

            auto& transform = reg.emplace<Transform>(entity);
            transform.position.x = (0.5f - static_cast<float>(width * (1 + space)) / 2.f) * tileSize;
            transform.position.y = 0;
            transform.position.z = zPos;

            auto& wall = reg.emplace<PingPongWall>(entity);
            wall.isLeft = true;
        }

        // Right
        {
            auto entity = reg.create();
            auto& col = reg.emplace<SimpleCollision>(entity);
            col.size.x = tileSize;
            col.size.y = tileSize * (height - 2) * (1 + space);

            auto& transform = reg.emplace<Transform>(entity);
            transform.position.x = (0.5f + (width - 1) + (width - 1) * space - static_cast<float>(width * (1 + space)) / 2.f) * tileSize;
            transform.position.y = 0;
            transform.position.z = zPos;

            auto& wall = reg.emplace<PingPongWall>(entity);
            wall.isLeft = false;
        }
    }

    // Ball
    CreatePingPongBall(tileSize, ColorRGBA(1, 1, 1, 1), { 7,-7,0 }, { -1,5,zPos });
    //CreatePingPongBall(reg, TileSize, Color(0.5f, 1, 1, 1), { -14,14,0 },   { 1,3,zPos });
    CreatePingPongBall(tileSize, ColorRGBA(1, 0.5f, 1, 1), { -6,4,0 }, { -1,1,zPos });
    //CreatePingPongBall(reg, TileSize, Color(1, 1, 0.5f, 1), {- 7,-7,0 },    { 1,-1,zPos });
    //CreatePingPongBall(reg, TileSize, Color(0.5f, 0.5f, 1, 1), { 20,14,0 }, { -1,-3,zPos });
    //CreatePingPongBall(reg, TileSize, Color(0.5f, 0.5f, 0.5f, 1), { -14,-20,0 }, { 1,-5,zPos });
    CreatePingPongBall(tileSize, ColorRGBA(0.5f, 1, 0.5f, 1), { 8,8,0 }, { -1,-7,zPos });

    // Player controller setup
    const Float32 playerSize = tileSize * ((height - 2) * (1 + space) * 0.33f);
    PingPongPlayerInputSystem::SetupPlayerBoarders(playerSize, -playerSize);
    PingPongPlayerInputSystem::s_PlayerSpeed = tileSize * 14.f;
    PingPongAISystem::s_AIPlayerSpeed = tileSize * 14.f;
    // 1st player
    {
        auto entity = reg.create();
        auto& col = reg.emplace<SimpleCollision>(entity);
        col.size.x = tileSize;
        col.size.y = playerSize;

        auto& transform = reg.emplace<Transform>(entity);
        transform.position.x = (2.5f - static_cast<float>(width * (1 + space)) / 2.f) * tileSize;
        transform.position.y = 0;
        transform.position.z = zPos;

        auto& sprite = reg.emplace<Sprite>(entity);
        AssignSprite(sprite, "EmptyWhitePixel");
        sprite.size.x = tileSize;
        sprite.size.y = playerSize;

        //auto& controller = reg.emplace<ControllerMapping>(entity);
        //PingPongPlayerInputSystem::SetupPlayerOneInput(controller);
        auto& ai = reg.emplace<AI>(entity);
        ai.side = EPlayerSide::LEFT;
    }

    // 2nd player
    {
        auto entity = reg.create();
        auto& col = reg.emplace<SimpleCollision>(entity);
        col.size.x = tileSize;
        col.size.y = playerSize;

        auto& transform = reg.emplace<Transform>(entity);
        transform.position.x = (0.5f + (width - 3) + (width - 1) * space - static_cast<float>(width * (1 + space)) / 2.f) * tileSize;
        transform.position.y = 0;
        transform.position.z = zPos;

        auto& sprite = reg.emplace<Sprite>(entity);
        AssignSprite(sprite, "EmptyWhitePixel");
        sprite.size.x = tileSize;
        sprite.size.y = playerSize;

        auto& controller = reg.emplace<ControllerMapping>(entity);
        PingPongPlayerInputSystem::SetupPlayerTwoInput(controller);
        //auto& ai = reg.emplace<AI>(entity);
        //ai.side = EPlayerSide::RIGHT;
    }

    PingPongAISystem::SetupPlayerBoarders(playerSize, -playerSize, playerSize);

    // Score text
    auto scoreLeft = reg.create();
    auto& playerScoreLeft = reg.emplace<PlayerScore>(scoreLeft);
    playerScoreLeft.isLeft = true;
    auto& textLeft = reg.emplace<Text>(scoreLeft);
    textLeft.spacing = 0.6f;

    auto scoreRight = reg.create();
    auto& playerScoreRight = reg.emplace<PlayerScore>(scoreRight);
    playerScoreRight.isLeft = false;
    auto& textRight = reg.emplace<Text>(scoreRight);
    textRight.spacing = 0.6f;

    // add score system to count scores for left and right collisions
    PlayerScoresSystem::SetFieldSize(width, height, tileSize * (1 + space));
}
