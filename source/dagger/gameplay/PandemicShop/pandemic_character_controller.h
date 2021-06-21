#pragma once

#include "core/core.h"
#include "core/system.h"
#include "gameplay/PandemicShop/pandemic_character_controller_fsm.h"

using namespace dagger;

namespace pandemic {
struct PandemicCharacter {
  int speed{1};
  EDirection direction{ EDirection::Down };
};

class PandemicControllerSystem : public System {
  PandemicCharacterControllerFSM characterFSM;

public:
  String SystemName() override { return "Character Controller System"; }

  void Run() override;
};
} // namespace pandemic