#pragma once

#include "core/core.h"
#include "core/engine.h"

#include "core/network/server.h"
#include "core/network/message.h"
#include "multiplayer_messages.h"

#include <unordered_map>

using namespace dagger;

namespace multiplayer
{
    struct PlayerData
    {
        Vector3 position;
        Vector4 color;
    };

    class MultiplayerServer : public NetworkServer<EMultiplayerMessage>
    {
    public:
        MultiplayerServer(UInt16 port_) : NetworkServer<EMultiplayerMessage>(port_) 
        {

        };

        void Run();

    protected:
        bool OnClientConnect(std::shared_ptr<Connection> client) override;
        void OnClientDisconnect(std::shared_ptr<Connection> client) override;
        void OnMessage(std::shared_ptr<Connection> client, Message<EMultiplayerMessage>& message) override;
        void OnClientValidated(std::shared_ptr<Connection> client) override;

    private:
        std::unordered_map<UInt32, PlayerData> m_playerData;
    };
} // namespace multiplayer