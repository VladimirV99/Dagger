#include "gameplay/platformer/platformer_main.h"

#include "core/audio.h"
#include "core/core.h"
#include "core/engine.h"
#include "core/game/transforms.h"
#include "core/graphics/animation.h"
#include "core/graphics/animations.h"
#include "core/graphics/camera.h"
#include "core/graphics/gui.h"
#include "core/graphics/shaders.h"
#include "core/graphics/sprite.h"
#include "core/graphics/sprite_render.h"
#include "core/graphics/text.h"
#include "core/graphics/textures.h"
#include "core/input/inputs.h"
#include "gameplay/platformer/camera_focus.h"
#include "gameplay/platformer/parallax.h"
#include "gameplay/platformer/platformer_controller.h"
#include "tools/diagnostics.h"

using namespace dagger;
using namespace platformer;

void Platformer::GameplaySystemsSetup()
{
	auto& engine = Engine::Instance();

	engine.AddSystem<PlatformerControllerSystem>();
	engine.AddSystem<ParallaxSystem>();
	engine.AddSystem<CameraFollowSystem>();
}

void SetCamera()
{
	auto* camera = Engine::GetDefaultResource<Camera>();
	camera->mode = ECameraMode::FixedResolution;
	camera->size = {800, 600};
	camera->zoom = 2;
	camera->position = {0, 0, 0};
	camera->Update();
}

struct Character
{
	Entity entity;
	Sprite& sprite;
	Animator& animator;
	InputReceiver& input;
	PlatformerCharacter& character;

	static Character Get(Entity entity_)
	{
		auto& reg = Engine::Registry();
		auto& sprite = reg.get_or_emplace<Sprite>(entity_);
		auto& anim = reg.get_or_emplace<Animator>(entity_);
		auto& input = reg.get_or_emplace<InputReceiver>(entity_);
		auto& character = reg.get_or_emplace<PlatformerCharacter>(entity_);

		return Character {entity_, sprite, anim, input, character};
	}

	static Character Create(String input_ = "", ColorRGB color_ = {1, 1, 1}, Vector2 position_ = {0, 0})
	{
		auto& reg = Engine::Registry();
		auto entity = reg.create();

		ATTACH_TO_FSM(CharacterControllerFSM, entity);

		auto chr = Character::Get(entity);

		chr.sprite.scale = {1, 1};
		chr.sprite.position = {position_, 0.0f};
		chr.sprite.color = {color_, 1.0f};

		AssignSprite(chr.sprite, "souls_like_knight_character:IDLE:idle1");
		AnimatorPlay(chr.animator, "souls_like_knight_character:IDLE");

		if (!input_.empty())
			chr.input.contexts.push_back(input_);

		chr.character.speed = 50;

		return chr;
	}
};

void CreateBackdrop()
{
	auto& reg = Engine::Registry();
	auto* camera = Engine::GetDefaultResource<Camera>();

	/* Create terrain */ {
		auto back = reg.create();
		auto& sprite = reg.get_or_emplace<Sprite>(back);

		AssignSprite(sprite, "EmptyWhitePixel");
		sprite.color = {0, 0, 0, 1};
		sprite.size = {200, 200};
		sprite.scale = {10, 1};
		sprite.position = {0, -125, 1};
	}

	/* Put background image */ {
		auto entity = reg.create();
		auto& sprite = reg.get_or_emplace<Sprite>(entity);

		AssignSprite(sprite, "souls_like_knight_character:BACKGROUND:Background");
		sprite.position.z = 10;
	}

	/* Put grass */ {
		auto entity = reg.create();
		auto& sprite = reg.get_or_emplace<Sprite>(entity);
		auto& parallax = reg.get_or_emplace<Parallax>(entity);
		parallax.lastCameraPosition = camera->position;
		parallax.strength = 0.5f;

		AssignSprite(sprite, "souls_like_knight_character:BACKGROUND:Grass");
		sprite.position = {0, -25, 5};
	}

	/* Put trees */ {
		auto entity = reg.create();
		auto& sprite = reg.get_or_emplace<Sprite>(entity);
		auto& parallax = reg.get_or_emplace<Parallax>(entity);
		parallax.lastCameraPosition = camera->position;
		parallax.strength = 0.25f;

		AssignSprite(sprite, "souls_like_knight_character:BACKGROUND:Tree");
		sprite.position = {0, 30, 7};
	}

	{
		auto ui = reg.create();
		auto& text = reg.emplace<Text>(ui);
		text.spacing = 0.6f;
		text.Set("pixel-font", "hello world");
	}
}

void Platformer::WorldSetup()
{
	Engine::GetDefaultResource<Audio>()->PlayLoop("music");

	SetCamera();
	CreateBackdrop();

	auto mainChar = Character::Create("ASDW", {1, 1, 1}, {-100, 0});
	Engine::Registry().emplace<CameraFollowFocus>(mainChar.entity);

	auto sndChar = Character::Create("Arrows", {1, 0, 0}, {100, 0});
	Engine::Registry().emplace<CameraFollowFocus>(sndChar.entity);
}
