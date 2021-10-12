#include "multiplayer_server.h"

using namespace dagger;
using namespace multiplayer;

void MultiplayerServer::Run()
{
	Start();
	while(true)
	{
		Update(-1, true);
	}
}

bool MultiplayerServer::CanClientConnect(asio::ip::tcp::endpoint endpoint_)
{
	return true;
}

void MultiplayerServer::OnClientConnected(UInt32 clientId_)
{

}

void MultiplayerServer::OnClientValidated(UInt32 clientId_)
{
	Message<EMultiplayerMessage> message (EMultiplayerMessage::AcceptClient);
	Send(clientId_, message);
}

void MultiplayerServer::OnClientDisconnected(UInt32 clientId_)
{
	if (m_playerData.find(clientId_) != m_playerData.end())
	{
		Message<EMultiplayerMessage> message {EMultiplayerMessage::RemovePlayer};
		message.header.sender = clientId_;
		Broadcast(message, clientId_);
		m_playerData.erase(clientId_);
	}
}

void MultiplayerServer::OnMessage(UInt32 clientId_, Message<EMultiplayerMessage>& message_)
{
	switch(message_.header.type)
	{
		case EMultiplayerMessage::AcceptClient:
			break;
		case EMultiplayerMessage::AddPlayer:
		{
			Broadcast(message_, clientId_);
			for (const auto& player : m_playerData)
			{
				Message<EMultiplayerMessage> message (EMultiplayerMessage::AddPlayer);
				message.header.sender = player.first;
				message << player.second.position;
				message << player.second.color;
				Send(clientId_, message);
			}
			PlayerData data;
			message_ >> data.color >> data.position;
			m_playerData[message_.header.sender] = data;
			Logger::info("Added new player");
			break;
		}
		case EMultiplayerMessage::UpdatePlayer:
		{
			Broadcast(message_, clientId_);
			message_ >> m_playerData[message_.header.sender].position;
			break;
		}
		case EMultiplayerMessage::RemovePlayer:
		{
			Broadcast(message_, clientId_);
			Logger::info("Removed player");
			break;
		}
	}
}