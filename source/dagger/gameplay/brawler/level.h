#pragma once

#include "core/core.h"
#include "core/engine.h"
#include "gameplay/brawler/entities/character.h"

using namespace dagger;

namespace brawler {

    enum PlatformType 
    {
        EMPTY,
        BLOCK
    };

    using Tilemap = std::vector<std::vector<PlatformType>>;
    using TileCoords = std::pair<int, int>;

    class Level
    {
    public:
        static constexpr float TILE_WIDTH = 20.0f;
        static constexpr float TILE_HEIGHT = 20.0f;

        static constexpr unsigned LEVEL_WIDTH = 20;
        static constexpr unsigned LEVEL_HEIGHT = 15;

        static void Create();
        
        static TileCoords WorldToTile(Vector2 worldPos);
        static Vector2 TileToWorld(int x, int y);
        inline static float TileToWorldX(int x);
        inline static float TileToWorldY(int y);

        static PlatformType getTile(int x, int y);
        static std::optional<float> getGround(BrawlerCharacter c);
    private:
        static Tilemap tiles;
    };

}