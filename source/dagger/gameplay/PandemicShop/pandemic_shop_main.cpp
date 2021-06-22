#include "pandemic_shop_main.h"

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

#include "tools/diagnostics.h"

#include "gameplay/common/simple_collisions.h"
#include "gameplay/ping_pong/pingpong_ball.h"
#include "gameplay/ping_pong/player_scores.h"
#include "gameplay/ping_pong/pingpong_tools.h"
#include "gameplay/PandemicShop/pandemic_player_input.h"
#include "gameplay/PandemicShop/pandemic_tools.h"
#include "gameplay/PandemicShop/pandemic_character_controller.h"
#include "gameplay/PandemicShop/karen_controller.h"
#include "gameplay/PandemicShop/item.h"
#include "gameplay/PandemicShop/player.h"
#include "gameplay/PandemicShop/karen.h"
#include "gameplay/PandemicShop/ai_system.h"

#include "gameplay/PandemicShop/level.h"
#include "gameplay/PandemicShop/level_system.h"
#include "gameplay/PandemicShop/game_menu.h"
#include "core/graphics/text.h"


using namespace dagger;
using namespace pandemic_shop;
//---------------------------
using namespace pandemic;


void PandemicShopGame::CoreSystemsSetup()
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
    // engine.AddSystem<CollisionDetectionSystem>();
    // engine.AddSystem<GUISystem>();
    // engine.AddSystem<ToolMenuSystem>();
#endif //!defined(NDEBUG)
}

void PandemicShopGame::GameplaySystemsSetup()
{
    auto& engine = Engine::Instance();

    engine.AddPausableSystem<SimpleCollisionsSystem>();
    engine.AddPausableSystem<PandemicControllerSystem>();
    engine.AddPausableSystem<KarenControllerSystem>();
    engine.AddPausableSystem<CollisionDetectionSystem>();
    engine.AddPausableSystem<Pickable>();
    engine.AddPausableSystem<AISystem>();

    engine.AddSystem<LevelSystem>();
    engine.AddPausableSystem<GameMenuSystem>();
#if defined(DAGGER_DEBUG)
  
#endif // defined(DAGGER_DEBUG)
}

void PandemicShopGame::WorldSetup()
{
    auto* camera = Engine::GetDefaultResource<Camera>();
    camera->mode = ECameraMode::FixedResolution;

    camera->size = { 1280, 720 };
    camera->zoom = 1.5;
    camera->position = { 0, 0, 0 };
    camera->Update();
    pandemic_shop::SetupStartScreen();
}

void pandemic_shop::SetupWorld(std::string level)
{

    auto& engine = Engine::Instance();
    auto& reg = engine.Registry();
    Level::Load(level);

}

//-------------------------------------------pocetni ekran i restartekran-------------------------------------
void pandemic_shop::SetupStartScreen() {
  auto& engine = Engine::Instance();
  auto& reg = engine.Registry();

  auto entity = reg.create();
  auto &sprite = reg.emplace<Sprite>(entity);
  sprite.position.z = 0;

  auto &gmb = reg.emplace<GameMenuButton>(entity);
  auto &input = reg.emplace<InputReceiver>(entity);
  input.contexts.push_back("menu");
  gmb.position = sprite.position;

  AssignSprite(sprite, "PandemicShop:start_button");
  gmb.size = sprite.size;

  //text
  auto entityt = reg.create();
  auto &text = reg.emplace<Text>(entityt);
  text.spacing = 0.6f;
  text.letterSize = {37.0f, 47.0f};
  text.Set("pixel-font","Pandemic Shop Game", {10, 175, 98});

  auto entityt2 = reg.create();
  auto &text2 = reg.emplace<Text>(entityt2);
  text2.letterSize = {37.0f, 47.0f};
  text2.spacing = 0.5f;
  text2.Set("pixel-font", "Start", {10, 100, 98});

  auto entityt3 = reg.create();
  auto &text3 = reg.emplace<Text>(entityt3);
  text3.spacing = 0.4f;
  text3.letterSize = {20.0f, 30.0f};
  text3.Set("pixel-font", "Avoid Karens and try to pick up", {10, -100, 98});

  auto entityt4 = reg.create();
  auto &text4 = reg.emplace<Text>(entityt4);
  text4.spacing = 0.4f;
  text4.letterSize = {20.0f, 30.0f};
  text4.Set("pixel-font", "as many groceries as possible !", {10, -150, 98});
}

void pandemic_shop::SetupRestartScreen(int number_of_collected_items_,
                                       int number_of_items_,
                                       bool victory) {
    auto& engine = Engine::Instance();
    auto &reg = engine.Registry();
    reg.clear();

    auto entity = reg.create();
    auto &sprite = reg.emplace<Sprite>(entity);
    sprite.position.z = 0;

    auto &gmb = reg.emplace<GameMenuButton>(entity);
    auto &input = reg.emplace<InputReceiver>(entity);
    input.contexts.push_back("menu");
    gmb.position = sprite.position;

    AssignSprite(sprite, "PandemicShop:replay_button");
    gmb.size = sprite.size;

    auto entityt = reg.create();
    auto &text = reg.emplace<Text>(entityt);
    text.spacing = 0.6f;
    text.alignment = TextAlignment::CENTER;
    text.letterSize = {37.0f, 47.0f};

    if(victory){
      text.Set("pixel-font", "Victory!", {10, 175, 98});
      auto entityt1 = reg.create();
      auto &text1 = reg.emplace<Text>(entityt1);
      text1.spacing = 0.6f;
      text1.letterSize = {37.0f, 47.0f};
      text1.Set("pixel-font", 
                "Collected " + std::to_string((int)number_of_collected_items_) + "/" + 
                  std::to_string((int)number_of_items_)+ " items", {10, 100, 98});
    }
    else{
      text.Set("pixel-font", "Game Over!", {10, 175, 98});    
            auto entityt1 = reg.create();
      auto &text1 = reg.emplace<Text>(entityt1);
      text1.spacing = 0.6f;
      text1.letterSize = {37.0f, 47.0f};
      text1.Set("pixel-font",
                "Collected " + std::to_string((int)number_of_collected_items_) +
                    "/" + std::to_string((int)number_of_items_) + " items",
                {10, 100, 98});
    }
    
    
}
