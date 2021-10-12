#pragma once

#include "core/core.h"
#include "core/engine.h"

#include "core/network/message.h"
#include "core/network/client.h"
#include "multiplayer_messages.h"

#include <unordered_map>
#include "entt/entt.hpp"

using namespace dagger;

namespace multiplayer
{
	struct RemotePlayerTarget
	{
		Vector3 startPosition;
		Vector3 endPosition;
		TimePoint startTime;
	};

	class ClientEventSystem : public System
	{
	public:
		inline String SystemName() const override
		{
			return "Client Event System";
		}

		void SpinUp() override;
		void WindDown() override;
		void Run() override;

		void OnKeyboardEvent(KeyboardEvent kEvent_);
		void OnNetworkEvent(NetworkEvent event_);
		void OnNetworkMessage(Message<EMultiplayerMessage> message_);

	private:
		UInt32 m_playerId;
		std::unordered_map<UInt32, Entity> m_players;
		Float32 m_elapsedTime = 0.0f;
	};
} // namespace multiplayer