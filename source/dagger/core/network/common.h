#pragma once

#ifdef _WIN32
#define _WIN32_WINNT 0x0A00
#endif
#define ASIO_STANDALONE
#include <asio.hpp>
#include <asio/ts/buffer.hpp>
#include <asio/ts/internet.hpp>

#define MAX_MESSAGE_SIZE 1024
#define MAX_CLIENT_PROCESS_MESSAGE 10

using namespace dagger;

enum class ENetworkEventType
{
	Connected,
	Validated,
	Disconnected
};

struct NetworkEvent
{
	ENetworkEventType type;
	UInt32 clientId;
	String host;
	UInt16 port;
};