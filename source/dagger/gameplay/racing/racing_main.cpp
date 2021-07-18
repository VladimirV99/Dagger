#include "racing_main.h"

#include "core/core.h"
#include "core/engine.h"
#include "core/game/transforms.h"
#include "core/graphics/animation.h"
#include "core/graphics/shaders.h"
#include "core/graphics/sprite.h"
#include "core/graphics/window.h"
#include "core/input/inputs.h"
#include "gameplay/common/particles.h"
#include "gameplay/common/simple_collisions.h"
#include "gameplay/racing/racing_car.h"
#include "gameplay/racing/racing_game_logic.h"
#include "gameplay/racing/racing_player_car.h"

using namespace dagger;
using namespace racing_game;

void RacingGame::GameplaySystemsSetup()
{
	auto& engine = Engine::Instance();

	engine.AddSystem<RacingPlayerInputSystem>();
	engine.AddSystem<RacingCarSystem>();
	engine.AddSystem<RacingCollisionsLogicSystem>();
	engine.AddSystem<SimpleCollisionsSystem>();
	engine.AddSystem<common_res::ParticleSystem>();
}

void RacingGame::WorldSetup()
{
	ShaderSystem::Use("standard");

	auto* camera = Engine::GetDefaultResource<Camera>();
	camera->mode = ECameraMode::FixedResolution;
	camera->size = {600, 600};
	camera->zoom = 1;
	camera->position = {0, 0, 0};
	camera->Update();

	racing_game::SetupWorld();
}

void racing_game::SetupWorld()
{
	auto& reg = Engine::Registry();

	constexpr Vector2 scale(1, 1);

	constexpr int heigh = 30;
	constexpr int width = 21;
	constexpr float tileSize = 20.0f;

	{
		auto entity = reg.create();
		auto& fieldSettings = reg.emplace<RacingGameFieldSettings>(entity);
		fieldSettings.fieldWidth = width;
		fieldSettings.fieldHeight = heigh;
		fieldSettings.fieldTileSize = tileSize;

		Engine::PutDefaultResource<RacingGameFieldSettings>(&fieldSettings);
	}

	float zPos = 1.f;

	for (int i = 0; i < heigh; i++)
	{
		for (int j = 0; j < width; j++)
		{
			auto entity = reg.create();
			auto& sprite = reg.emplace<Sprite>(entity);
			AssignSprite(sprite, "EmptyWhitePixel");
			sprite.size = scale * tileSize;

			sprite.color = {0.4f, 0.4f, 0.4f, 1};

			if ((j == width / 2) || (width % 2 == 0 && (j == width / 2 - 1)))
			{
				sprite.color = {1, 1, 1, 1};
			}

			if (j == 0 || j == width - 1)
			{
				sprite.color = {0, 0, 0, 1};
			}

			auto& transform = reg.emplace<Transform>(entity);
			transform.position.x = (0.5f + j - static_cast<float>(width) / 2.f) * tileSize;
			transform.position.y = (0.5f + i - static_cast<float>(heigh) / 2.f) * tileSize;
			transform.position.z = zPos;
		}
	}

	zPos -= 0.5f;

	// player
	{
		auto entity = reg.create();
		auto& sprite = reg.emplace<Sprite>(entity);
		AssignSprite(sprite, "Racing:police-car-bmw-z4");
		float ratio = sprite.size.y / sprite.size.x;
		sprite.size = {2 * tileSize, 2 * tileSize * ratio};

		auto& transform = reg.emplace<Transform>(entity);
		transform.position = {-tileSize * 4, -tileSize * 4, zPos};

		auto& racingPlayer = reg.emplace<RacingPlayerCar>(entity);
		racingPlayer.horzSpeed = tileSize * 6;

		reg.emplace<ControllerMapping>(entity);

		auto& col = reg.emplace<SimpleCollision>(entity);
		col.size = sprite.size;

		common_res::ParticleSpawnerSettings settings;
		settings.Setup(
			0.05f, {4.f, 4.f}, {-0.2f, -1.4f}, {0.2f, -0.6f}, {0.6f, 0.6f, 0.6f, 1}, {1, 1, 1, 1}, "EmptyWhitePixel");
		common_res::ParticleSystem::SetupParticleSystem(entity, settings);
	}

	// collisions for road bounds

	// other cars
	int amountOfCars = rand() % 3 + 3;
	for (int i = 0; i < amountOfCars; i++)
	{
		auto entity = reg.create();
		auto& sprite = reg.emplace<Sprite>(entity);
		AssignSprite(sprite, "Racing:police-car-bmw-z4");
		float ratio = sprite.size.y / sprite.size.x;
		sprite.size = {2 * tileSize, 2 * tileSize * ratio};
		sprite.scale.y = -1;

		auto& transform = reg.emplace<Transform>(entity);
		transform.position = {tileSize * (3 * (i + 1) - width / 2), tileSize * (-i * 2 + heigh / 2), zPos};

		auto& racingCar = reg.emplace<RacingCar>(entity);
		racingCar.speed = tileSize * (rand() % 5 + 3);

		auto& col = reg.emplace<SimpleCollision>(entity);
		col.size = sprite.size;

		common_res::ParticleSpawnerSettings settings;
		settings.Setup(
			0.1f, {4.f, 4.f}, {-0.2f, 0.4f}, {0.2f, 1.2f}, {0.6f, 0.6f, 0.6f, 1}, {1, 1, 1, 1}, "EmptyWhitePixel");
		common_res::ParticleSystem::SetupParticleSystem(entity, settings);
	}
}
