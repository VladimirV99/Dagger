#pragma once

#include "core/core.h"
#include "core/engine.h"
#include "core/game.h"
#include "core/system.h"

using namespace dagger;

namespace pandemic_shop {

void SetupWorld(std::string level);
void SetupStartScreen();
void SetupRestartScreen(int number_of_collected_items_, int number_of_items_, bool victory);

    class PandemicShopGame : public Game
    {
        inline String GetIniFile() override
        {
            return "pandemic.ini";
        };

  void CoreSystemsSetup() override;
  void GameplaySystemsSetup() override;
  void WorldSetup() override;
};
} // namespace pandemic_shop
