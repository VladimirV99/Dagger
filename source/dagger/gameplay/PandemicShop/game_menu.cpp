#include "game_menu.h"

#include "core/core.h"
#include "core/engine.h"
#include "core/game/transforms.h"
#include "core/graphics/animation.h"
#include "core/graphics/shaders.h"
#include "core/graphics/sprite.h"
#include "core/graphics/window.h"
#include "core/input/inputs.h"
#include "core/graphics/text.h"
#include "gameplay/PandemicShop/player.h"

#include "gameplay/PandemicShop/pandemic_shop_main.h"

using namespace pandemic;
using namespace pandemic_shop;
using namespace dagger;

bool GameMenuSystem::s_GameOver = false;
String GameMenuSystem::s_Winner = "";

void GameMenuSystem::Run() {
  auto view = Engine::Registry().view<GameMenuButton, Sprite, InputReceiver>();

  for (auto entity : view) {
    auto &gmb = view.get<GameMenuButton>(entity);
    auto &sprite = view.get<Sprite>(entity);
    auto &input = view.get<InputReceiver>(entity);

    if (IsMouseOver(gmb)) {
      if (EPSILON_NOT_ZERO(input.Get("start"))) {
        m_LoadGame = true;
      } 
    } 
  }
}

void GameMenuSystem::SpinUp() {
  
  Engine::Dispatcher().sink<NextFrame>().connect<&GameMenuSystem::OnEndOfFrame>(
      this);
}

void GameMenuSystem::WindDown() {
  Engine::Dispatcher().sink<NextFrame>().disconnect<&GameMenuSystem::OnEndOfFrame>(this);
}

void GameMenuSystem::OnEndOfFrame() {
  if (m_LoadGame) {
    m_LoadGame = false;
    Engine::Registry().clear();
    if(level == 1){
      pandemic_shop::SetupWorld(Engine::Instance(), "next");
    }
    else{
      pandemic_shop::SetupWorld(Engine::Instance(), "default");
    }
    
  }
  else if (s_GameOver) {
      s_GameOver = false;
      if(!restarted){
        auto hero_view = Engine::Registry().view<PandemicCharacter>();
        auto entity = hero_view.begin();

        auto chr = Character::Get(*entity);  
        
        collected_items = Pickable::picked;
        
        Logger::info("collected items {}", collected_items);
        if(collected_items < 18){
          pandemic_shop::SetupRestartScreen(Engine::Instance(), collected_items, 36, false);
        }
        else{
          pandemic_shop::SetupRestartScreen(Engine::Instance(), collected_items, 36, true);
          level++;
        }
        restarted = true;
      }
      else{
        restarted = false;
      }
      Pickable::picked = 0;
      collected_items = 0;
      
    }  
    if (Engine::s_IsPaused) {
        Engine::s_IsPaused = false;
        s_GameOver = true;
        return;
     }
  }

bool GameMenuSystem::IsMouseOver(GameMenuButton gmb_) {
  auto cursor = dagger::Input::CursorPositionInWorld();
  Vector2 b(gmb_.position.x + gmb_.size.x * (-0.5),
            gmb_.position.y + gmb_.size.y * (-0.5));

  if (cursor.x < b.x + gmb_.size.x && cursor.x > b.x &&
      cursor.y < b.y + gmb_.size.y && cursor.y > b.y) {
    return true;
  }

  return false;
}

void GameMenuSystem::EndOfGame(String winner_) {
  s_GameOver = true;
  s_Winner = winner_;
}