#include "core/input/inputs.h"
#include "core/graphics/sprite.h"
#include "core/graphics/animation.h"
#include "core/game/transforms.h"
#include "core/graphics/sprite_render.h"
#include "core/graphics/textures.h"
#include "core/graphics/animations.h"

#include "gameplay/common/simple_collisions.h"
#include "gameplay/PandemicShop/karen_controller.h"
#include "gameplay/PandemicShop/item.h"
#include "gameplay/PandemicShop/ai_system.h"

using namespace pandemic;
struct KarenCharacter {
  Entity entity;
  Sprite &sprite;
  Animator &animator;
  InputReceiver &input;
  PandemicKarenCharacter &character;
  Transform &transform;
  SimpleCollision &collision;
  AICommand &command;
  std::vector<Entity> inventory;
  


  static KarenCharacter Get(Entity entity) {
    auto &reg = Engine::Registry();
    auto &sprite = reg.get_or_emplace<Sprite>(entity);
    auto &anim = reg.get_or_emplace<Animator>(entity);
    auto &input = reg.get_or_emplace<InputReceiver>(entity);
    auto &character = reg.get_or_emplace<PandemicKarenCharacter>(entity);
    auto &transform = reg.get_or_emplace<Transform>(entity);
    auto &collision = reg.get_or_emplace<SimpleCollision>(entity);
    auto &command = reg.get_or_emplace<AICommand>(entity);
    
    return KarenCharacter{entity, sprite, anim, input, character, transform, collision, command};
  }

  static KarenCharacter Create(String input_ = "", ColorRGB color_ = {1, 1, 1},
                          Vector2 position_ = {0, 0}) {
    auto &reg = Engine::Registry();
    auto entity = reg.create();

    ATTACH_TO_FSM(KarenControllerFSM, entity);

    auto chr = KarenCharacter::Get(entity);

    chr.transform.position = {position_, 0.0f};

    chr.collision.size = {32, 32};

    chr.sprite.scale = {2, 2};
    chr.sprite.position = {position_, 0.0f};
    chr.sprite.color = {color_, 1.0f};
    reg.emplace<CollisionType::Char>(entity);

    chr.command.previous = position_;
    chr.command.current = position_;
    chr.command.pick_probability = rand() / (float)RAND_MAX;

    AssignSprite(chr.sprite, "PandemicShop:AMELIA_IDLE:FRONT:amelia_idle1");
    AnimatorPlay(chr.animator, "PandemicShop:AMELIA_IDLE_FRONT");

    if (input_ != "")
      chr.input.contexts.push_back(input_);

    chr.character.speed = 200;

    return chr;
  }
  
};