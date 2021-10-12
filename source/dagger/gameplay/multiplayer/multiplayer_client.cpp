#include "multiplayer_client.h"

#include "core/network/client.h"
#include "multiplayer_client_events.h"
#include "core/graphics/camera.h"
#include "multiplayer_messages.h"

using namespace dagger;
using namespace multiplayer;

void MultiplayerGame::GameplaySystemsSetup()
{
	auto& engine = Engine::Instance();

	engine.AddSystem<NetworkClientSystem<EMultiplayerMessage>>("127.0.0.1", 5000);
	engine.AddSystem<ClientEventSystem>();
}

void MultiplayerGame::WorldSetup()
{
	auto* camera = Engine::GetDefaultResource<Camera>();
	camera->mode = ECameraMode::FixedResolution;
	camera->size = {800, 600};
	camera->zoom = 1;
	camera->position = {0, 0, 0};
	camera->Update();

	// SetupWorld();
}