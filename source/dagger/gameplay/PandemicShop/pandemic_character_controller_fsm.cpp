#include "gameplay/PandemicShop/pandemic_character_controller_fsm.h"
#include "gameplay/PandemicShop/pandemic_character_controller.h"

#include "core/core.h"
#include "core/engine.h"
#include "core/graphics/animation.h"
#include "core/graphics/sprite.h"
#include "core/input/inputs.h"
#include "core/game/transforms.h"

using namespace dagger;
using namespace pandemic;

// Idle

void PandemicCharacterControllerFSM::Idle::Enter(
    PandemicCharacterControllerFSM::StateComponent &state_) {

    auto&& [animator, character] = Engine::Registry().get<Animator, pandemic::PandemicCharacter>(state_.entity);
    AnimatorPlay(animator, "PandemicShop:IDLE" + GetNameFromDirection(character.direction));
}

void PandemicCharacterControllerFSM::Idle::Run(PandemicCharacterControllerFSM::StateComponent &state_) {
    
    auto& input = Engine::Registry().get<InputReceiver>(state_.entity);

    auto h = input.Get("horizontal");
    auto v = input.Get("vertical");

    if (EPSILON_NOT_ZERO(h) || EPSILON_NOT_ZERO(v)) {
        GoTo(EPandemicCharacterStates::Running, state_);
    }
}

DEFAULT_EXIT(PandemicCharacterControllerFSM, Idle);

// Running

void PandemicCharacterControllerFSM::Running::Run(
    PandemicCharacterControllerFSM::StateComponent &state_) 
{
  auto &&[animator, input, character, transform] =
        Engine::Registry().get<Animator, InputReceiver, pandemic::PandemicCharacter, Transform>(state_.entity);

    auto h = input.Get("horizontal");
    auto v = input.Get("vertical");

    switch (character.direction)
    {
    case EDirection::Left:
      transform.position.x += character.speed * (-1) * Engine::DeltaTime();
      break;
    case EDirection::Right:
      transform.position.x += character.speed * (1) * Engine::DeltaTime();
      break;
    case EDirection::Up:
      transform.position.y += character.speed * (-1) * Engine::DeltaTime();
      break;
    case EDirection::Down:
      transform.position.y += character.speed * (1) * Engine::DeltaTime();
      break;

    default:
      break;
    }

    if (EPSILON_ZERO(h) && EPSILON_ZERO(v)) 
    {
        GoTo(EPandemicCharacterStates::Idle, state_);
    } else {
        auto direction = GetDirectionFromVector(Vector2{ h, v });
        if (direction != character.direction)
        {
            character.direction = direction;
            AnimatorPlay(animator, "PandemicShop:RUN" + GetNameFromDirection(direction));
        }
    }
}

void PandemicCharacterControllerFSM::Running::Enter(
    PandemicCharacterControllerFSM::StateComponent& state_) {}

void PandemicCharacterControllerFSM::Running::Exit(
    PandemicCharacterControllerFSM::StateComponent& state_) {}
