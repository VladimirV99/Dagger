#pragma once
#include "core/core.h"
#include "core/system.h"

using namespace dagger;

namespace pandemic {
enum struct EGameMenuType { StartScreen, RestartScreen };

struct GameMenuButton {
  EGameMenuType type{EGameMenuType::StartScreen};
  Vector2 size{0, 0};
  Vector3 position{0, 0, 0};
};

class GameMenuSystem : public System {
  bool m_LoadGame = false;
  static bool s_GameOver;
  static String s_Winner;
  static inline bool restarted = false;
  static inline unsigned level = 0;
  

public:
  inline String SystemName() { return "Game Menu System"; }
  static inline int collected_items = 0;
  void SpinUp() override;
  void WindDown() override;
  void Run() override;

  bool IsMouseOver(GameMenuButton gmb_);
  static void EndOfGame(String winner_);

private:
  void OnEndOfFrame();
};
} // namespace pandemic