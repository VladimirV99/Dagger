#pragma once

#include "core/core.h"
#include "core/system.h"
#include "gameplay/PandemicShop/karen_controller_fsm.h"

using namespace dagger;

namespace pandemic {
struct PandemicKarenCharacter {
    int speed{1};
    EDirection direction{ EDirection::Down };
  

};

class KarenControllerSystem : public System {
  KarenControllerFSM characterFSM;

public:
  String SystemName() override { return "Character Controller System"; }

  void Run() override;
};
} // namespace pandemic