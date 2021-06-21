#include "core/input/inputs.h"
#include "core/graphics/sprite.h"
#include "core/graphics/animation.h"
#include "core/game/transforms.h"
#include "core/graphics/sprite_render.h"
#include "core/graphics/textures.h"
#include "core/graphics/animations.h"

#include "gameplay/common/simple_collisions.h"
#include "gameplay/PandemicShop/pandemic_player_input.h"
#include "gameplay/PandemicShop/pandemic_character_controller.h"
#include "gameplay/PandemicShop/item.h"

using namespace pandemic;

struct Character {
  Entity entity;
  Sprite &sprite;
  Animator &animator;
  InputReceiver &input;
  PandemicCharacter &character;
  Transform &transform;
  SimpleCollision &collision;
  //Maro ovo treba da se pormeni kada je kraj igrice
  bool isGameOver;


  static Character Get(Entity entity) {
    auto &reg = Engine::Registry();
    auto &sprite = reg.get_or_emplace<Sprite>(entity);
    auto &anim = reg.get_or_emplace<Animator>(entity);
    auto &input = reg.get_or_emplace<InputReceiver>(entity);
    auto &character = reg.get_or_emplace<PandemicCharacter>(entity);
    auto &transform = reg.get_or_emplace<Transform>(entity);
    auto &collision = reg.get_or_emplace<SimpleCollision>(entity);
    
    
    //return Character{entity, sprite, anim, input};
    return Character{entity, sprite, anim, input, character, transform, collision};
  }

  static Character Create(String input_ = "", ColorRGB color_ = {1, 1, 1},
                          Vector2 position_ = {0, 0}) {
    auto &reg = Engine::Registry();
    auto entity = reg.create();

    ATTACH_TO_FSM(PandemicCharacterControllerFSM, entity);

    auto chr = Character::Get(entity);

    chr.transform.position = {position_, 0.0f};

    chr.collision.size = {32, 32};

    chr.sprite.scale = {2, 2};
    chr.sprite.position = {position_, 0.0f};
    chr.sprite.color = {color_, 1.0f};
    reg.emplace<CollisionType::Char>(entity);

    chr.isGameOver = false;

    AssignSprite(chr.sprite, "PandemicShop:BOB_IDLE:FRONT:bob_idle1");
    AnimatorPlay(chr.animator, "PandemicShop:IDLE_FRONT");

    if (input_ != "")
      chr.input.contexts.push_back(input_);

    chr.character.speed = 200;

    return chr;
  }
  
};