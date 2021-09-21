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

bool MultiplayerServer::OnClientConnect(std::shared_ptr<Connection> client)
{
    return true;
}

void MultiplayerServer::OnClientDisconnect(std::shared_ptr<Connection> client)
{
    // TODO Handle disconnecting. Messages can't be sent from this function
    // if (client && m_playerData.find(client->GetId()) != m_playerData.end())
    // {
    //     Message<EMultiplayerMessage> message;
    //     message.header.id = EMultiplayerMessage::RemovePlayer;
    //     message << client->GetId();
    //     Broadcast(message, client);
    //     m_playerData.erase(client->GetId());
    // }
}

void MultiplayerServer::OnMessage(std::shared_ptr<Connection> client_, Message<EMultiplayerMessage>& message_)
{
    switch(message_.header.id)
    {
        case EMultiplayerMessage::AcceptClient:
            break;
        case EMultiplayerMessage::AddPlayer:
        {
            Broadcast(message_, client_);
            for (const auto& player : m_playerData)
            {
                Message<EMultiplayerMessage> message (EMultiplayerMessage::AddPlayer);
                message << player.second.position;
                message << player.second.color;
                message << player.first;
                Send(client_, message);
            }
            PlayerData data;
            UInt32 id;
            message_ >> id >> data.color >> data.position;
            m_playerData[client_->GetId()] = data;
            Logger::info("Added new player");
            break;
        }
        case EMultiplayerMessage::UpdatePlayer:
        {
            Broadcast(message_, client_);
            UInt32 id;
            message_ >> id >> m_playerData[client_->GetId()].position;
            break;
        }
        case EMultiplayerMessage::RemovePlayer:
        {
            Broadcast(message_, client_);
            Logger::info("Removed player");
            break;
        }
    }
}

void MultiplayerServer::OnClientValidated(std::shared_ptr<Connection> client)
{
    Message<EMultiplayerMessage> message (EMultiplayerMessage::AcceptClient);
    message << client->GetId();
    Send(client, message);
}