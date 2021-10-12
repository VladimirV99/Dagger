#pragma once

#include "common.h"

#include "core/core.h"
#include "core/engine.h"
#include "core/system.h"

#include "message.h"
#include "concurrent_queue.h"

#include <thread>

using namespace dagger;

template <typename Archetype>
class NetworkClientSystem : public System
{
private:
	class Connection
	{
	public:
		Connection(asio::io_context& context_, ConcurrentQueue<Message<Archetype>>& messageInput_, ConcurrentQueue<ENetworkEventType>& events_) 
			: m_context(context_), m_socket(context_), m_messageInput(messageInput_), m_events(events_)
		{
			m_handshakeIn = 0;
			m_handshakeOut = 0;
		}

		~Connection()
		{
			Disconnect();
		}

		UInt32 GetId() const
		{
			return m_id;
		}

		void ConnectToServer(const asio::ip::tcp::resolver::results_type& endpoints)
		{
			asio::async_connect(m_socket, endpoints,
			[this](std::error_code error, asio::ip::tcp::endpoint endpoint)
			{
				if (!error)
				{
					m_events.Push(ENetworkEventType::Connected);
					ReadValidation();
				}
				else
				{
					Logger::error("Connection To Server Failed");
				}
			});
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
				Logger::warn("Error sending message. Client not authenticated");
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
			m_events.Push(ENetworkEventType::Disconnected);
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
						m_messageInput.Push(m_tempMessageIn);
						ReadHeader();
					}
				}
				else
				{
					Logger::error("Failed Reading Message Header");
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
					m_messageInput.Push(m_tempMessageIn);
					ReadHeader();
				}
				else
				{
					Logger::error("Failed Reading Message Body");
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
					Logger::error("Failed Writing Message Header");
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
					Logger::error("Failed Writing Message Body");
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

		/* async */ void ReadValidation()
		{
			asio::async_read(m_socket, asio::buffer(&m_handshakeIn, sizeof(UInt64)),
			[this](std::error_code error, std::size_t length){
				if(!error)
				{
					m_handshakeOut = scramble(m_handshakeIn);
					WriteValidation();
				}
				else
				{
					Logger::error("Failed Reading Validation Message");
					CloseConnection();
				}
			});
		}

		/* async */ void WriteValidation()
		{
			asio::async_write(m_socket, asio::buffer(&m_handshakeOut, sizeof(UInt64)),
			[this](std::error_code error, std::size_t length){
				if(!error)
				{
					ReadId();
				}
				else
				{
					Logger::error("Failed Writing Validation Message");
					CloseConnection();
				}
			});
		}

		/* async */ void ReadId()
		{
			asio::async_read(m_socket, asio::buffer(&m_id, sizeof(m_id)),
			[this](std::error_code error, std::size_t length){
				if(!error)
				{
					m_isAuthenticated = true;
					m_events.Push(ENetworkEventType::Validated);
					ReadHeader();
				}
				else
				{
					Logger::error("Failed Reading Id Message");
					CloseConnection();
				}
			});
		}

	private:
		UInt32 m_id;

		asio::io_context& m_context;
		asio::ip::tcp::socket m_socket;

		ConcurrentQueue<ENetworkEventType>& m_events;
		ConcurrentQueue<Message<Archetype>>& m_messageInput;
		ConcurrentQueue<Message<Archetype>> m_messageOutput;
		Message<Archetype> m_tempMessageIn;

		bool m_isAuthenticated { false };
		UInt64 m_handshakeIn { 0 };
		UInt64 m_handshakeOut { 0 };
	};

public:
	NetworkClientSystem(const String& host_, const UInt16 port_)
		: m_host(host_), m_port(port_)
	{

	}

	virtual ~NetworkClientSystem()
	{
		
	}

	bool Connect()
	{
		try
		{
			asio::ip::tcp::resolver resolver(m_context);
			auto endpoints = resolver.resolve(m_host, std::to_string(m_port));

			m_connection = std::make_unique<Connection>(
				m_context,
				m_messageInput,
				m_events
			);

			m_connection->ConnectToServer(endpoints);

			m_contextThread = std::thread([this](){m_context.run();});
		}
		catch (std::exception& e)
		{
			Logger::error("Network Client Exception: {}", e.what());
			return false;
		}
		return true;
	}

	void Disconnect()
	{
		if(IsConnected())
			m_connection->Disconnect();

		m_context.stop();

		if(m_contextThread.joinable())
			m_contextThread.join();

		m_connection.release();
	}

	bool IsConnected()
	{
		if (m_connection)
			return m_connection->IsConnected();
		else
			return false;
	}

	void Send(const Message<Archetype>& message_)
	{
		if (IsConnected())
			m_connection->Send(message_);
	}

	ConcurrentQueue<Message<Archetype>>& Incoming()
	{
		return m_messageInput;
	}

	inline String SystemName() const override
	{
		return "Network Client System";
	}

	void SpinUp() override
	{
		Connect();
	}

	void Run() override
	{
		// TODO is this needed ?
		/*if (!IsConnected())
		{
			Logger::error("Disconnected from server");
			Engine::Dispatcher().trigger<Exit>();
			return;
		}*/

		unsigned processedMessages = 0;
		while (processedMessages < MAX_CLIENT_PROCESS_MESSAGE)
		{
			if (!m_events.Empty())
			{
				auto event = m_events.Pop();
				Engine::Dispatcher().trigger<NetworkEvent>(event, m_connection->GetId(), m_host, m_port);
				if(event == ENetworkEventType::Disconnected) 
				{ 
					m_messageInput.Clear();
					break;
				}
				continue;
			}

			if (!m_messageInput.Empty()) 
			{
				auto message = Incoming().Pop();
				Engine::Dispatcher().trigger<Message<Archetype>>(message);

				processedMessages++;
				continue;
			}

			break;
		}
	}

	void WindDown() override
	{
		Disconnect();
	}

private:
	String m_host;
	UInt16 m_port;

	asio::io_context m_context;
	std::thread m_contextThread;
	std::unique_ptr<Connection> m_connection;

	ConcurrentQueue<ENetworkEventType> m_events;
	ConcurrentQueue<Message<Archetype>> m_messageInput;
};