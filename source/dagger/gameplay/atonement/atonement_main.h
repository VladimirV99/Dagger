#pragma once

#include "core/core.h"
#include "core/system.h"
#include "core/game.h"
#include "core/engine.h"
#include "tools/toolmenu.h"
#include "gameplay/editor/savegame_system.h"
#include "gameplay/editor/editor_defs.h"
#include "core/graphics/tool_render.h"

using namespace dagger;

namespace atonement
{
    class AtonementGame : public Game, public SaveLoadProcess<ECommonSaveArchetype>
    {
        inline String GetIniFile() override
        {
            return "atonement.ini";
        };

        void CoreSystemsSetup() override;
        void GameplaySystemsSetup() override;
        void WorldSetup() override;

        ECommonSaveArchetype Save(Entity entity_, JSON::json& saveTo_) override;
        void Load(ECommonSaveArchetype archetype_, Entity entity_, JSON::json& loadFrom_) override;
        public:
        static void RestartGame();
    };
}
