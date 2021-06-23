#include "char_controller_fsm.h"
#include "core/core.h"
#include "core/engine.h"
#include "core/input/inputs.h"
#include "core/graphics/sprite.h"
#include "core/graphics/animation.h"
#include "core/game/transforms.h"
#include <gameplay/atonement/atonement_controller.h>

#include "gameplay/common/cooldown_manager.h"
#include "gameplay/atonement/systems/character_collisions.h"

#include <iostream>

using namespace dagger;
using namespace atonement;

// Idle

void CharControllerFSM::Idle::Enter(CharControllerFSM::StateComponent& state_)
{
	std::cout << "[IDLE]" << std::endl;
	auto& animator = Engine::Registry().get<Animator>(state_.entity);
	AnimatorPlay(animator, "BlueWizard:IDLE");
}

DEFAULT_EXIT(CharControllerFSM, Idle);

void CharControllerFSM::Idle::Run(CharControllerFSM::StateComponent& state_)
{
	auto& input = Engine::Registry().get<InputReceiver>(state_.entity);

	if (EPSILON_NOT_ZERO(input.Get("walk")))
	{
		GoTo(ECharStates::Walking, state_);
	}

	if (EPSILON_NOT_ZERO(input.Get("jump")))
	{
		GoTo(ECharStates::JumpWindup, state_);
	}

	if (EPSILON_NOT_ZERO(input.Get("dash")) && canDash(state_.entity))
	{
		GoTo(ECharStates::Dashing, state_);
	}

	auto& character = Engine::Registry().get<AtonementController::AtonementCharacter>(state_.entity);
	if (!character.grounded) {
		GoTo(ECharStates::JumpWinddown, state_);
	}
}


// Walking

void CharControllerFSM::Walking::Enter(CharControllerFSM::StateComponent& state_)
{
	std::cout << "[WALK]" << std::endl;
	auto& animator = Engine::Registry().get<Animator>(state_.entity);
	AnimatorPlay(animator, "BlueWizard:WALK");
}

// same as: DEFAULT_EXIT(CharacterControllerFSM, Walking);
void CharControllerFSM::Walking::Exit(CharControllerFSM::StateComponent& state_)
{}

void CharControllerFSM::Walking::Run(CharControllerFSM::StateComponent& state_)
{
	static Float32 movedInLastFrame = 0;

	auto&& [transform, sprite, input, character, collision] = Engine::Registry().get<Transform, Sprite, InputReceiver, AtonementController::AtonementCharacter, CharacterCollision>(state_.entity);

	Float32 walk = input.Get("walk");

	if (EPSILON_ZERO(walk))
	{
		GoTo(ECharStates::Idle, state_);
	}
	else
	{
		if((sprite.scale.x < 0 && walk > 0) || (sprite.scale.x > 0 && walk < 0)){
			sprite.scale.x *= -1;
		}

		if((collision.collidedLeft && sprite.scale.x < 0) || (collision.collidedRight && sprite.scale.x > 0)){
			transform.position.x -= movedInLastFrame;
			movedInLastFrame = 0;
		}
		else {
			movedInLastFrame = character.speed * sprite.scale.x * Engine::DeltaTime();
			transform.position.x += movedInLastFrame;
		}

	}

	if (EPSILON_NOT_ZERO(input.Get("jump")))
	{
		GoTo(ECharStates::JumpWindup, state_);
	}

	if (EPSILON_NOT_ZERO(input.Get("dash")) && canDash(state_.entity))
	{
		GoTo(ECharStates::Dashing, state_);
	}

	if (!character.grounded) {
		GoTo(ECharStates::JumpWinddown, state_);
	}
}

// Jump Windup

void CharControllerFSM::JumpWindup::Enter(CharControllerFSM::StateComponent& state_)
{
	std::cout << "[JUMP_WU]" << std::endl;
	auto& animator = Engine::Registry().get<Animator>(state_.entity);
	AnimatorPlayOnce(animator, "BlueWizard:JUMP_WINDUP");
}

DEFAULT_EXIT(CharControllerFSM, JumpWindup);

void CharControllerFSM::JumpWindup::Run(CharControllerFSM::StateComponent& state_)
{
	static Float32 movedInLastFrame = 0;
	static Float32 jumpedInLastFrame = 0;

	auto&& [transform, sprite, input, character, collision] = Engine::Registry().get<Transform, Sprite, InputReceiver, AtonementController::AtonementCharacter, CharacterCollision>(state_.entity);

	if (EPSILON_NOT_ZERO(input.Get("dash")) && canDash(state_.entity))
	{
		character.dashJumped = true;
		GoTo(ECharStates::Dashing, state_);
	}

	Float32 walk = input.Get("walk");

	if (!(EPSILON_ZERO(walk))) {

		if ((sprite.scale.x < 0 && walk > 0) || (sprite.scale.x > 0 && walk < 0)) {
			sprite.scale.x *= -1;
		}

		if ((collision.collidedLeft && sprite.scale.x < 0) || (collision.collidedRight && sprite.scale.x > 0)) {
			//transform.position.x -= movedInLastFrame;
			//movedInLastFrame = 0;
			auto cdManager = Engine::GetDefaultResource<CooldownManager<std::string>>();
			if (!cdManager->isOnCooldown(state_.entity, "wall jump")) {
				GoTo(ECharStates::WallJump, state_);
			}
		}
		else {
			movedInLastFrame = character.speed * sprite.scale.x * Engine::DeltaTime();
			transform.position.x += movedInLastFrame;
		}
	}

	if (character.jumpedHeight < character.jumpHeight && !collision.collidedUp) {
		Float32 tmp = character.jumpSpeed * sprite.scale.y * Engine::DeltaTime();
		transform.position.y += tmp;
		character.jumpedHeight += tmp;
	}
	else if (collision.collidedUp) {
		transform.position.y -= jumpedInLastFrame;
		jumpedInLastFrame = 0;
		character.jumpedHeight = 0;
		GoTo(ECharStates::JumpWinddown, state_);
	}
	else {
		jumpedInLastFrame = 0;
		character.jumpedHeight = 0;
		GoTo(ECharStates::JumpWinddown, state_);
	}
}

// Jump Winddown

void CharControllerFSM::JumpWinddown::Enter(CharControllerFSM::StateComponent& state_)
{
	std::cout << "[JUMP_WD]" << std::endl;
	auto&& animator = Engine::Registry().get<Animator>(state_.entity);
	AnimatorPlayOnce(animator, "BlueWizard:JUMP_WINDDOWN");
	
	auto&& character = Engine::Registry().get<AtonementController::AtonementCharacter>(state_.entity);

	character.fallingAnimationEnded = false;
}

DEFAULT_EXIT(CharControllerFSM, JumpWinddown);

void CharControllerFSM::JumpWinddown::Run(CharControllerFSM::StateComponent& state_)
{
	static Float32 movedInLastFrame = 0;
	static Float32 fallenInLastFrame = 0;

	auto&& [transform, sprite, input, character, collision] = Engine::Registry().get<Transform, Sprite, InputReceiver, AtonementController::AtonementCharacter, CharacterCollision>(state_.entity);

	if (EPSILON_NOT_ZERO(input.Get("dash")) && canDash(state_.entity))
	{
		character.dashJumped = true;
		GoTo(ECharStates::Dashing, state_);
	}

	Float32 walk = input.Get("walk");

	if (!(EPSILON_ZERO(walk))) {
		if ((sprite.scale.x < 0 && walk > 0) || (sprite.scale.x > 0 && walk < 0)) {
			sprite.scale.x *= -1;
		}

		if ((collision.collidedLeft && sprite.scale.x < 0) || (collision.collidedRight && sprite.scale.x > 0)) {
			//transform.position.x -= movedInLastFrame;
			//movedInLastFrame = 0;
			auto cdManager = Engine::GetDefaultResource<CooldownManager<std::string>>();
			if (!cdManager->isOnCooldown(state_.entity, "wall jump")) {
				GoTo(ECharStates::WallJump, state_);
			}
		}
		else {
			movedInLastFrame = character.speed * sprite.scale.x * Engine::DeltaTime();
			transform.position.x += movedInLastFrame;
		}
	}

	if (!character.grounded) {
		fallenInLastFrame = character.fallSpeed * sprite.scale.y * Engine::DeltaTime();
		transform.position.y -= fallenInLastFrame;
	}
	else if (character.fallingAnimationEnded) {
		transform.position.y += fallenInLastFrame;
		fallenInLastFrame = 0;
		GoTo(ECharStates::Idle, state_);
	}
}

// Dashing

void CharControllerFSM::Dashing::Enter(CharControllerFSM::StateComponent& state_)
{
	std::cout << "[DASH]" << std::endl;
	auto&& [character, animator] = Engine::Registry().get<AtonementController::AtonementCharacter, Animator>(state_.entity);
	AnimatorPlayOnce(animator, "BlueWizard:DASH");

	character.dashingAnimationEnded = false;

	auto cdManager = Engine::GetDefaultResource<CooldownManager<std::string>>();
	cdManager->registerCooldown(state_.entity, "dash", character.dashCooldown);
}

DEFAULT_EXIT(CharControllerFSM, Dashing);

void CharControllerFSM::Dashing::Run(CharControllerFSM::StateComponent& state_)
{
	static Float32 dashedInLastFrame = 0;

	auto&& [transform, sprite, character, collision] = Engine::Registry().get<Transform, Sprite, AtonementController::AtonementCharacter, CharacterCollision>(state_.entity);

	Bool collided = (collision.collidedLeft && sprite.scale.x < 0) || (collision.collidedRight && sprite.scale.x > 0);
	
	if (character.wallDashed) {
		sprite.scale.x *= -1;
	}

	if (collided && !character.wallDashed) {
		//transform.position.x -= dashedInLastFrame;
		dashedInLastFrame = 0;
		GoTo(ECharStates::WallJump, state_);
	}
	else {
		character.wallDashed = false; // HACK: if dash speed is not fast enough for char to get out of collider
									  // this might result in them returning to WallJump state
		dashedInLastFrame = character.dashSpeed * sprite.scale.x * Engine::DeltaTime();
		transform.position.x += dashedInLastFrame;
	}

	if (character.dashingAnimationEnded) {
		if (!character.grounded) {
			if (collided) {
				GoTo(ECharStates::WallJump, state_);
			}
			else {
				GoTo(ECharStates::JumpWinddown, state_);
			}
		}
		else {
			GoTo(ECharStates::Idle, state_);
		}
	}

}

// Wall Jump

void CharControllerFSM::WallJump::Enter(CharControllerFSM::StateComponent& state_)
{
	std::cout << "[WALL_JMP]" << std::endl;
	auto&& animator = Engine::Registry().get<Animator>(state_.entity);
	AnimatorPlayOnce(animator, "BlueWizard:WALL_JUMP");

	auto&& character = Engine::Registry().get<AtonementController::AtonementCharacter>(state_.entity);
	character.dashJumped = false;
}

DEFAULT_EXIT(CharControllerFSM, WallJump);

void CharControllerFSM::WallJump::Run(CharControllerFSM::StateComponent& state_)
{
	static Float32 movedInLastFrame = 0;
	static Float32 fallenInLastFrame = 0;
	static Bool jumping = false;

	auto&& [transform, sprite, input, character, collision] = Engine::Registry().get<Transform, Sprite, InputReceiver, AtonementController::AtonementCharacter, CharacterCollision>(state_.entity);

	if (EPSILON_NOT_ZERO(input.Get("dash")) && canDash(state_.entity))
	{
		character.dashJumped = true;
		character.wallDashed = true;
		GoTo(ECharStates::Dashing, state_);
	}

	if (EPSILON_NOT_ZERO(input.Get("wall")) && !jumping)
	{
		auto cdManager = Engine::GetDefaultResource<CooldownManager<std::string>>();
		if(!cdManager->isOnCooldown(state_.entity, "boost")){
			cdManager->registerCooldown(state_.entity, "boost", 0.3);
			auto&& animator = Engine::Registry().get<Animator>(state_.entity);
			AnimatorPlayOnce(animator, "BlueWizard:JUMP_WINDUP");
			sprite.scale.x *= -1;
			jumping = true;
		}
	}

	if(!jumping) {
		if (!((collision.collidedLeft && sprite.scale.x < 0) || (collision.collidedRight && sprite.scale.x > 0))) {
			movedInLastFrame = character.speed * sprite.scale.x * Engine::DeltaTime();
			transform.position.x += movedInLastFrame;
			if (character.grounded) {
				GoTo(ECharStates::JumpWinddown, state_);
			}
			else {
				GoTo(ECharStates::Idle, state_);
			}
		}

		if (!character.grounded) {
			fallenInLastFrame = character.fallSpeed * character.slideSpeedAsPercentOfFall * sprite.scale.y * Engine::DeltaTime();
			transform.position.y -= fallenInLastFrame;
		}
		else {
			transform.position.y += fallenInLastFrame;
			fallenInLastFrame = 0;
			GoTo(ECharStates::Idle, state_);
		}
	}
	else {
		auto cdManager = Engine::GetDefaultResource<CooldownManager<std::string>>();
		if (cdManager->isOnCooldown(state_.entity, "boost")) {
			movedInLastFrame = character.boostSide * sprite.scale.x * Engine::DeltaTime();
			transform.position.x += movedInLastFrame;
			fallenInLastFrame = character.boostUp * sprite.scale.y * Engine::DeltaTime();
			transform.position.y += fallenInLastFrame;

			if ((collision.collidedLeft && sprite.scale.x < 0) || (collision.collidedRight && sprite.scale.x > 0)) {
				jumping = false;
			}

			if (EPSILON_NOT_ZERO(input.Get("dash")) && canDash(state_.entity))
			{
				jumping = false;
				character.dashJumped = true;
				GoTo(ECharStates::Dashing, state_);
			}
		}
		else {
			jumping = false;
			GoTo(ECharStates::JumpWindup, state_);
		}
	}
}

/* Helper functions */

void CharControllerFSM::OnAnimationEnd(ViewPtr<Animation> animation) {
	Engine::Registry().view<CharControllerFSM::StateComponent>()
		.each([&](CharControllerFSM::StateComponent& state_)
			{
				String animName = animation.get()->name;
				auto&& character = Engine::Registry().get<AtonementController::AtonementCharacter>(state_.entity);

				if (animName == "BlueWizard:DASH") {
					character.dashingAnimationEnded = true;
				}
				else if (animName == "BlueWizard:JUMP_WINDDOWN") {
					character.fallingAnimationEnded = true;
				}
			});
}

Bool CharControllerFSM::canDash(Entity entity) {
	auto cdManager = Engine::GetDefaultResource<CooldownManager<std::string>>();
	auto&& character = Engine::Registry().get<AtonementController::AtonementCharacter>(entity);
	return !cdManager->isOnCooldown(entity, "dash") && !character.dashJumped;
}