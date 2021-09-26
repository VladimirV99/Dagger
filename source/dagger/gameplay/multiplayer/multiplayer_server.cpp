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
    message << clientId_;
    Send(clientId_, message);
}

void MultiplayerServer::OnClientDisconnected(UInt32 clientId_)
{
    // TODO Handle disconnecting. Messages can't be sent from this function
    if (m_playerData.find(clientId_) != m_playerData.end())
    {
        Message<EMultiplayerMessage> message;
        message.header.id = EMultiplayerMessage::RemovePlayer;
        message << clientId_;
        Broadcast(message, clientId_);
        m_playerData.erase(clientId_);
    }
}

void MultiplayerServer::OnMessage(UInt32 clientId_, Message<EMultiplayerMessage>& message_)
{
    switch(message_.header.id)
    {
        case EMultiplayerMessage::AcceptClient:
            break;
        case EMultiplayerMessage::AddPlayer:
        {
            Broadcast(message_, clientId_);
            for (const auto& player : m_playerData)
            {
                Message<EMultiplayerMessage> message (EMultiplayerMessage::AddPlayer);
                message << player.second.position;
                message << player.second.color;
                message << player.first;
                Send(clientId_, message);
            }
            PlayerData data;
            UInt32 id;
            message_ >> id >> data.color >> data.position;
            m_playerData[clientId_] = data;
            Logger::info("Added new player");
            break;
        }
        case EMultiplayerMessage::UpdatePlayer:
        {
            Broadcast(message_, clientId_);
            UInt32 id;
            message_ >> id >> m_playerData[clientId_].position;
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