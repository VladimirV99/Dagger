#include "multiplayer_client_events.h"

#include "core/game/transforms.h"
#include "core/graphics/sprite.h"
#include "core/input/inputs.h"

using namespace dagger;
using namespace multiplayer;

void ClientEventSystem::SpinUp()
{
	Engine::Dispatcher().sink<KeyboardEvent>().connect<&ClientEventSystem::OnKeyboardEvent>(this);
	Engine::Dispatcher().sink<NetworkEvent>().connect<&ClientEventSystem::OnNetworkEvent>(this);
	Engine::Dispatcher().sink<Message<EMultiplayerMessage>>().connect<&ClientEventSystem::OnNetworkMessage>(this);
}

void ClientEventSystem::WindDown()
{
	Engine::Dispatcher().sink<KeyboardEvent>().disconnect<&ClientEventSystem::OnKeyboardEvent>(this);
	Engine::Dispatcher().sink<NetworkEvent>().disconnect<&ClientEventSystem::OnNetworkEvent>(this);
	Engine::Dispatcher().sink<Message<EMultiplayerMessage>>().disconnect<&ClientEventSystem::OnNetworkMessage>(this);
}

void ClientEventSystem::Run()
{
	if (m_playerId == 0)
		return;
	
	float horizontalVelocity = 0.0f;
	float verticalVelocity = 0.0f;
	if (Input::IsInputDown(EDaggerKeyboard::KeyW))
	{
		verticalVelocity += 1.0f;
	}
	if (Input::IsInputDown(EDaggerKeyboard::KeyS))
	{
		verticalVelocity -= 1.0f;
	}
	if (Input::IsInputDown(EDaggerKeyboard::KeyA))
	{
		horizontalVelocity -= 1.0f;
	}
	if (Input::IsInputDown(EDaggerKeyboard::KeyD))
	{
		horizontalVelocity += 1.0f;
	}

	auto &transform = Engine::Registry().get<Transform>(m_players[m_playerId]);
	transform.position.x += horizontalVelocity * 200.0f * Engine::DeltaTime();
	transform.position.y += verticalVelocity * 200.0f * Engine::DeltaTime();

	m_elapsedTime += Engine::DeltaTime();
	if (m_elapsedTime > 1.0f/40)
	{
		m_elapsedTime = 0;
		auto *client = Engine::GetDefaultResource<NetworkClientSystem<EMultiplayerMessage>>();
		Message<EMultiplayerMessage> message (EMultiplayerMessage::UpdatePlayer);
		message << transform.position;
		client->Send(message);
	}

	Engine::Registry().view<Transform, RemotePlayerTarget>().each(
		[](auto& transform_, auto& target_) 
		{
			Duration dt = Engine::CurrentTime() - target_.startTime;
			transform_.position = target_.startPosition + (target_.endPosition - target_.startPosition) * dt.count() * 40.0f;
		});
}

void ClientEventSystem::OnKeyboardEvent(KeyboardEvent kEvent_)
{
	
}

void ClientEventSystem::OnNetworkEvent(NetworkEvent event_) {
	switch (event_.type)
	{
	case ENetworkEventType::Connected:
		Logger::info("connected");
		break;
	case ENetworkEventType::Validated:
		Logger::info("validated");
		m_playerId = event_.clientId;
		break;
	case ENetworkEventType::Disconnected:
		Logger::info("disconnected");
		break;
	}
}

void ClientEventSystem::OnNetworkMessage(Message<EMultiplayerMessage> message_)
{
	auto &reg = Engine::Registry();
	auto *client = Engine::GetDefaultResource<NetworkClientSystem<EMultiplayerMessage>>();

	switch(message_.header.type)
	{
		case EMultiplayerMessage::AcceptClient:
		{
			auto player = reg.create();
			m_players[m_playerId] = player;

			auto &playerTransform = reg.emplace<Transform>(player);
			playerTransform.position = { 0.0f, 0.0f, 0.0f };

			auto &playerSprite = reg.emplace<Sprite>(player);
			playerSprite.color = { rand() % 255 / 255.0f, rand() % 255 / 255.0f, rand() % 255 / 255.0f, 1.0f };
			AssignSprite(playerSprite, "PingPong:ball");
			playerSprite.size = {50.0f, 50.0f};

			Message<EMultiplayerMessage> message (EMultiplayerMessage::AddPlayer);
			message << playerTransform.position;
			message << playerSprite.color;

			client->Send(message);
			break;
		}
		case EMultiplayerMessage::AddPlayer:
		{
			Vector3 position;
			Vector4 color;
			message_ >> color >> position;

			auto newPlayer = reg.create();
			m_players[message_.header.sender] = newPlayer;

			auto &playerTransform = reg.emplace<Transform>(newPlayer);
			playerTransform.position = position;

			auto& playerTarget = reg.emplace<RemotePlayerTarget>(newPlayer);
			playerTarget.startPosition = position;
			playerTarget.endPosition = position;
			playerTarget.startTime = Engine::CurrentTime();

			auto &playerSprite = reg.emplace<Sprite>(newPlayer);
			playerSprite.color = color;
			AssignSprite(playerSprite, "PingPong:ball");
			playerSprite.size = {50.0f, 50.0f};

			break;
		}
		case EMultiplayerMessage::UpdatePlayer:
		{
			auto it = m_players.find(message_.header.sender);
			if (it == m_players.end()) 
				return;

			Vector3 position;
			message_ >> position;

			auto& playerTransform = reg.get<Transform>(it->second);

			auto &playerTarget = reg.get<RemotePlayerTarget>(it->second);
			playerTarget.startPosition = playerTransform.position;
			playerTarget.endPosition = position;
			playerTarget.startTime = Engine::CurrentTime();

			break;
		}
		case EMultiplayerMessage::RemovePlayer:
		{
			auto it = m_players.find(message_.header.sender);
			if (it == m_players.end())
				return;

			reg.destroy(it->second);
			m_players.erase(message_.header.sender);
			break;
		}
	}
}