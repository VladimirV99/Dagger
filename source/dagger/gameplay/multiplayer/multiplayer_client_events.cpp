#include "multiplayer_client_events.h"

#include "core/network/client.h"

#include "core/game/transforms.h"
#include "core/graphics/sprite.h"
#include "core/input/inputs.h"

using namespace dagger;
using namespace multiplayer;

void ClientEventSystem::SpinUp()
{
    Engine::Dispatcher().sink<KeyboardEvent>().connect<&ClientEventSystem::OnKeyboardEvent>(this);
    Engine::Dispatcher().sink<Message<EMultiplayerMessage>>().connect<&ClientEventSystem::OnNetworkMessage>(this);
}

void ClientEventSystem::WindDown()
{
    Engine::Dispatcher().sink<KeyboardEvent>().disconnect<&ClientEventSystem::OnKeyboardEvent>(this);
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
		message << m_playerId;
		client->Send(message);
	}
}

void ClientEventSystem::OnKeyboardEvent(KeyboardEvent kEvent_)
{
	
}

void ClientEventSystem::OnNetworkMessage(Message<EMultiplayerMessage> message_)
{
	auto &reg = Engine::Registry();
	auto *client = Engine::GetDefaultResource<NetworkClientSystem<EMultiplayerMessage>>();

    switch(message_.header.id)
    {
		case EMultiplayerMessage::AcceptClient:
		{
			message_ >> m_playerId;

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
			message << m_playerId;
			client->Send(message);
			break;
		}
        case EMultiplayerMessage::AddPlayer:
		{
			UInt32 id;
			Vector3 position;
			Vector4 color;
			message_ >> id >> color >> position;

			auto newPlayer = reg.create();
			m_players[id] = newPlayer;

			auto &playerTransform = reg.emplace<Transform>(newPlayer);
			playerTransform.position = position;

			auto &playerSprite = reg.emplace<Sprite>(newPlayer);
			playerSprite.color = color;
			AssignSprite(playerSprite, "PingPong:ball");
			playerSprite.size = {50.0f, 50.0f};

            break;
		}
        case EMultiplayerMessage::UpdatePlayer:
		{
			UInt32 id;
			Vector3 position;
			message_ >> id >> position;

			auto &playerTransform = reg.get<Transform>(m_players[id]);
			playerTransform.position = position;

            break;
		}
        case EMultiplayerMessage::RemovePlayer:
		{
			UInt32 id;
			message_ >> id;
			reg.destroy(m_players[id]);
			m_players.erase(id);
            break;
		}
    }
}