#pragma once

#include "core/core.h"
#include "core/engine.h"
#include "core/game.h"
#include "core/system.h"

using namespace dagger;

namespace pandemic_shop {

void SetupWorld(Engine &engine_, std::string level);
void SetupStartScreen(Engine &engine_);
void SetupRestartScreen(Engine &engine_, int number_of_collected_items_, int number_of_items_, bool victory);

    class PandemicShopGame : public Game
    {
        inline String GetIniFile() override
        {
            return "pandemic.ini";
        };

  void CoreSystemsSetup(Engine &engine_) override;
  void GameplaySystemsSetup(Engine &engine_) override;
  void WorldSetup(Engine &engine_) override;
};
} // namespace pandemic_shop
