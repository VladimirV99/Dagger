#pragma once

#include "core/core.h"
#include "core/engine.h"

#include "common.h"
#include "message.h"
#include "concurrent_queue.h"

#include <thread>
#include <mutex>
#include <chrono>
#include <unordered_map>

using namespace dagger;

template<typename Archetype>
struct OwnedMessage
{
	UInt64 id;
	UInt32 remote;
	Message<Archetype> message;

	friend std::ostream& operator<<(std::ostream& os, const OwnedMessage& message)
	{
		os << message.message;
		return os;
	}
};

struct ConnectionEvent
{
	UInt64 id;
	UInt32 remote;
	ENetworkEventType type;
};

template <typename Archetype>
class NetworkServer
{
protected:
	struct Input
	{
		Input() = default;
		Input(const Input& other_) = delete;

		void QueueMessage(UInt32 clientId_, Message<Archetype>& message_)
		{
			message_.header.sender = clientId_;
			messageQueue.Push({m_counter.fetch_add(1), clientId_, message_});
			std::unique_lock<std::mutex> ul(m_waitingMutex);
			m_waiting.notify_one();
		}

		void QueueEvent(UInt32 clientId_, const ENetworkEventType& event_)
		{
			eventQueue.Push({m_counter.fetch_add(1), clientId_, event_});
			std::unique_lock<std::mutex> ul(m_waitingMutex);
			m_waiting.notify_one();
		}

		void Wait()
		{
			while(eventQueue.Empty() && messageQueue.Empty())
			{
				std::unique_lock<std::mutex> ul(m_waitingMutex);
				m_waiting.wait(ul);
			}
		}

		ConcurrentQueue<ConnectionEvent> eventQueue;
		ConcurrentQueue<OwnedMessage<Archetype>> messageQueue;

	private:    
		std::condition_variable m_waiting;
		std::mutex m_waitingMutex;
		std::atomic<UInt64> m_counter;
	};   

	class Connection
	{
	public:
		Connection(asio::io_context& context_, asio::ip::tcp::socket socket_, Input& input_) 
			: m_context(context_), m_socket(std::move(socket_)), m_input(input_)
		{
			m_handshakeOut = UInt64(std::chrono::system_clock::now().time_since_epoch().count());
			m_handshakeCheck = scramble(m_handshakeOut);
		}

		~Connection() {
			Disconnect();
		}

		UInt32 GetId() const
		{
			return m_id;
		}

		void AuthenticateClient(UInt32 id_ = 0)
		{
			if (!m_isAuthenticated && IsConnected())
			{
				m_id = id_;

				WriteValidation();
			}
		}

		void Disconnect()
		{
			if(IsConnected())
			{
				asio::post(m_context, [this]() { CloseConnection(); });
			}
		}

		bool IsConnected() const
		{
			return m_socket.is_open();
		}

		void Send(const Message<Archetype>& message)
		{
			if(!m_isAuthenticated)
			{
				Logger::warn("Error sending message. Client not authenticated: {}", m_id);
				return;
			}
			asio::post(m_context, [this, message](){
				bool isWritingMessage = !m_messageOutput.Empty();
				m_messageOutput.Push(message);
				if (!isWritingMessage)
					WriteHeader();
			});
		}

	private:
		void CloseConnection()
		{
			m_socket.close();
			m_input.QueueEvent(GetId(), ENetworkEventType::Disconnected);
			m_isAuthenticated = false;
		}

		/* async */ void ReadHeader()
		{
			asio::async_read(m_socket, asio::buffer(&m_tempMessageIn.header, sizeof(typename Message<Archetype>::Header)),
			[this](std::error_code error, std::size_t length){
				if(!error)
				{
					if(m_tempMessageIn.header.Size() > 0)
					{
						if (m_tempMessageIn.header.Size() > MAX_MESSAGE_SIZE)
						{
							Logger::error("Message Too Large. Closing Connection");
							CloseConnection();
						}
						else
						{
							m_tempMessageIn.body.resize(m_tempMessageIn.header.Size());
							ReadBody();
						}
					}
					else
					{
						m_input.QueueMessage(GetId(), m_tempMessageIn);
						ReadHeader();
					}
				}
				else
				{
					Logger::debug("Failed Reading Message Header: {}", m_id);
					CloseConnection();
				}
			});
		}

		/* async */ void ReadBody()
		{
			asio::async_read(m_socket, asio::buffer(m_tempMessageIn.body.data(), m_tempMessageIn.body.size()),
			[this](std::error_code error, std::size_t length){
				if(!error)
				{
					m_input.QueueMessage(GetId(), m_tempMessageIn);
					ReadHeader();
				}
				else
				{
					Logger::debug("Failed Reading Message Body: {}", m_id);
					CloseConnection();
				}
			});
		}

		/* async */ void WriteHeader()
		{
			if (m_messageOutput.Front().header.Size() > MAX_MESSAGE_SIZE)
			{
				Logger::warn("Message Too Large. Skipping");
				m_messageOutput.Pop();
				if(!m_messageOutput.Empty())
				{
					WriteHeader();
				}
			}
			asio::async_write(m_socket, asio::buffer(&m_messageOutput.Front().header, sizeof(typename Message<Archetype>::Header)),
			[this](std::error_code error, std::size_t length)
			{
				if(!error)
				{
					if(m_messageOutput.Front().body.size() > 0)
					{
						WriteBody();
					}
					else
					{
						m_messageOutput.Pop();
						if(!m_messageOutput.Empty())
						{
							WriteHeader();
						}
					}
				}
				else
				{
					Logger::debug("Failed Writing Message Header: {}", m_id);
					CloseConnection();
				}
			});
		}

		/* async */ void WriteBody()
		{
			asio::async_write(m_socket, asio::buffer(m_messageOutput.Front().body.data(), m_messageOutput.Front().body.size()),
			[this](std::error_code error, std::size_t length)
			{
				if(!error)
				{
					m_messageOutput.Pop();
					if(!m_messageOutput.Empty())
					{
						WriteHeader();
					}
				}
				else
				{
					Logger::debug("Failed Writing Message Body: {}", m_id);
					CloseConnection();
				}
			});
		}

		UInt64 scramble(UInt64 input_)
		{
			UInt64 out = input_ ^ 0x0123456789ABCDEF;
			out = (out & 0xF0F0F0F0F0F0F0) >> 4 | (out & 0x0F0F0F0F0F0F0F) << 4;
			return out ^ 0xFEDCBA9876543210;
		}

		/* async */ void WriteValidation()
		{
			asio::async_write(m_socket, asio::buffer(&m_handshakeOut, sizeof(UInt64)),
			[this](std::error_code error, std::size_t length){
				if(!error)
				{
					ReadValidation();
				}
				else
				{
					Logger::debug("Failed Writing Validation Message: {}", m_id);
					CloseConnection();
				}
			});
		}

		/* async */ void ReadValidation()
		{
			asio::async_read(m_socket, asio::buffer(&m_handshakeIn, sizeof(UInt64)),
			[this](std::error_code error, std::size_t length){
				if(!error)
				{
					if (m_handshakeIn == m_handshakeCheck)
					{
						m_isAuthenticated = true;
						m_input.QueueEvent(GetId(), ENetworkEventType::Validated);

						WriteId();
					}
					else
					{
						Logger::debug("Connection Validation Failed: {}", m_id);
						CloseConnection();
					}
				}
				else
				{
					Logger::debug("Failed Writing Validation Message: {}", m_id);
					CloseConnection();
				}
			});
		}

		/* async */ void WriteId()
		{
			asio::async_write(m_socket, asio::buffer(&m_id, sizeof(m_id)),
			[this](std::error_code error, std::size_t length){
				if(!error)
				{
					ReadHeader();
				}
				else
				{
					Logger::debug("Failed Writing Id Message: {}", m_id);
					CloseConnection();
				}
			});
		}

	private:
		UInt32 m_id;

		asio::io_context& m_context;
		asio::ip::tcp::socket m_socket;

		Input& m_input;
		Message<Archetype> m_tempMessageIn;
		ConcurrentQueue<Message<Archetype>> m_messageOutput;

		bool m_isAuthenticated { false };
		UInt64 m_handshakeOut { 0 };
		UInt64 m_handshakeIn { 0 };
		UInt64 m_handshakeCheck { 0 };
	};

public:
	NetworkServer(UInt16 port_)
		: m_acceptor(m_context, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port_))
	{

	}

	virtual ~NetworkServer()
	{
		Stop();
	}

	bool Start()
	{
		try
		{
			WaitForConnection();
			m_contextThread = std::thread([this](){m_context.run();});
		}
		catch(std::exception& e)
		{
			Logger::warn("Server Exception: {}", e.what());
			return false;
		}

		Logger::info("Server Started");
		return true;
	}

	void Stop()
	{
		m_context.stop();

		if(m_contextThread.joinable())
			m_contextThread.join();

		m_connections.clear();

		Logger::info("Server Stopped");
	}
	
	/* async */ void WaitForConnection()
	{
		m_acceptor.async_accept(
			[this](std::error_code error, asio::ip::tcp::socket socket)
			{
				if(!error)
				{
					Logger::debug("New Connection: {}:{}", socket.remote_endpoint().address().to_string(), socket.remote_endpoint().port());

					if(CanClientConnect(socket.remote_endpoint()))
					{
						std::unique_ptr<Connection> newConnection = std::make_unique<Connection>( 
							m_context, std::move(socket), m_input
						);
						newConnection->AuthenticateClient(m_idCounter++);

						m_input.QueueEvent(newConnection->GetId(), ENetworkEventType::Connected);

						// TODO Thread safety
						m_connectionMutex.lock();
						m_connections.insert(std::make_pair<UInt32, std::unique_ptr<Connection>>(
							newConnection->GetId(),
							std::move(newConnection))
						);
						m_connectionMutex.unlock();
					}
					else
					{
						Logger::debug("Connection Denied");
					}
				}
				else
				{
					Logger::error("New Connection Error: {}", error.message());
				}

				WaitForConnection();
			});
	}

	void Send(UInt32 clientId_, const Message<Archetype>& message_)
	{
		m_connectionMutex.lock();
		auto it = m_connections.find(clientId_);
		if (it != m_connections.end())
		{
			auto& client = it->second;
			m_connectionMutex.unlock();
			if (client && client->IsConnected())
			{
				client->Send(message_);
			}
		}
	}

	void Broadcast(const Message<Archetype>& message_, UInt32 ignore_ = nullptr) 
	{
		std::scoped_lock lock(m_connectionMutex);
		for (auto it = m_connections.begin(); it != m_connections.end();)
		{
			auto& client = it->second;
			if (client && client->IsConnected())
			{
				if (it->first != ignore_)
					client->Send(message_);
				++it;
			}
		}
	}

	bool isEventNext() 
	{
		if (!m_input.messageQueue.Empty() && !m_input.eventQueue.Empty())
		{
			return m_input.eventQueue.Front().id < m_input.messageQueue.Front().id;
		}

		return !m_input.eventQueue.Empty();
	}

	void ProcessEvent(const ConnectionEvent& event_)
	{
		switch (event_.type)
		{
		case ENetworkEventType::Connected:
			Logger::info("Client Connected: {}", event_.remote);
			OnClientConnected(event_.remote);
			break;
		case ENetworkEventType::Validated:
			Logger::info("Client Validated: {}", event_.remote);
			OnClientValidated(event_.remote);
			break;
		case ENetworkEventType::Disconnected:
			auto it = m_connections.find(event_.remote);
			if (it != m_connections.end())
			{
				Logger::info("Client Disconnected: {}", event_.remote);
				it->second.reset();
				m_connections.erase(it);
				OnClientDisconnected(event_.remote);
			}
			break;
		}
	}

	void Update(UInt16 maxMessages_ = -1, bool wait_ = false)
	{
		// Block until a message is received
		if(wait_)
		{
			m_input.Wait();
		}

		UInt16 messageCount = 0;
		while (messageCount < maxMessages_ && (!m_input.messageQueue.Empty() || !m_input.eventQueue.Empty()))
		{
			if (isEventNext())
			{
				auto event = m_input.eventQueue.Pop();
				ProcessEvent(event);
			}
			else
			{
				auto message = m_input.messageQueue.Pop();
				OnMessage(message.remote, message.message);
			}

			messageCount++;
		}
	}

protected:
	// Return whether the client should be allowed to connect
	// Used to implement client banning
	virtual bool CanClientConnect(asio::ip::tcp::endpoint endpoint_)
	{
		return true;
	}

	virtual void OnClientConnected(UInt32 clientId_)
	{
		
	}

	virtual void OnClientValidated(UInt32 clientId_)
	{

	}

	virtual void OnClientDisconnected(UInt32 clientId_)
	{

	}

	// TODO Use events
	virtual void OnMessage(UInt32 clientId_, Message<Archetype>& message_)
	{

	}

private:
	Input m_input;

	std::unordered_map<UInt32, std::unique_ptr<Connection>> m_connections;
	std::mutex m_connectionMutex;

	asio::io_context m_context;
	std::thread m_contextThread;

	asio::ip::tcp::acceptor m_acceptor;

	UInt32 m_idCounter = 10000;
};
