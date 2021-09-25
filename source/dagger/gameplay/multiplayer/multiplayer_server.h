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
        bool CanClientConnect(asio::ip::tcp::endpoint endpoint_) override;
        void OnClientDisconnect(UInt32 clientId_) override;
        void OnMessage(UInt32 clientId_, Message<EMultiplayerMessage>& message_) override;
        void OnClientValidated(UInt32 clientId_) override;

    private:
        std::unordered_map<UInt32, PlayerData> m_playerData;
    };
} // namespace multiplayer