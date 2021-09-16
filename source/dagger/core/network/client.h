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
        Connection(asio::io_context& context_, ConcurrentQueue<Message<Archetype>>& messageIn_) 
            : m_context(context_), m_socket(context_), m_messageInput(messageIn_)
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
                asio::post(m_context, [this](){m_socket.close();});
        }

        bool IsConnected() const
        {
            return m_socket.is_open();
        }

        void Send(const Message<Archetype>& message)
        {
            asio::post(m_context, [this, message](){
                bool isWritingMessage = !m_messageOutput.Empty();
                m_messageOutput.Push(message);
                if (!isWritingMessage)
                    WriteHeader();
            });
        }

    private:

        /* async */ void ReadHeader()
        {
            asio::async_read(m_socket, asio::buffer(&m_tempMessageIn.header, sizeof(MessageHeader<Archetype>)),
            [this](std::error_code error, std::size_t length){
                if(!error)
                {
                    if(m_tempMessageIn.header.size > 0)
                    {
                        if (m_tempMessageIn.header.size > MAX_MESSAGE_SIZE)
                        {
                            Logger::error("Message Too Large. Closing Connection");
                            m_socket.close();
                        }
                        else
                        {
                            m_tempMessageIn.body.resize(m_tempMessageIn.header.size);
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
                    m_socket.close();
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
                    m_socket.close();
                }
            });
        }

        /* async */ void WriteHeader()
        {
            if (m_messageOutput.Front().header.size > MAX_MESSAGE_SIZE)
            {
                Logger::warn("Message Too Large. Skipping");
                m_messageOutput.Pop();
                if(!m_messageOutput.Empty())
                {
                    WriteHeader();
                }
            }
            asio::async_write(m_socket, asio::buffer(&m_messageOutput.Front().header, sizeof(MessageHeader<Archetype>)),
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
                    m_socket.close();
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
                    m_socket.close();
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
                    ReadHeader();
                }
                else
                {
                    Logger::error("Failed Writing Validation Message");
                    m_socket.close();
                }
            });
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
                    m_socket.close();
                }
            });
        }

    private:
        UInt32 m_id;

        asio::io_context& m_context;
        asio::ip::tcp::socket m_socket;

        ConcurrentQueue<Message<Archetype>> m_messageOutput;
        ConcurrentQueue<Message<Archetype>>& m_messageInput;
        Message<Archetype> m_tempMessageIn;

        UInt64 m_handshakeIn = 0;
        UInt64 m_handshakeOut = 0;
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
                m_messageInput
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
        if (!IsConnected())
        {
            Logger::error("Disconnected from server");
            // TODO shutdown
            return;
        }

        unsigned processedMessages = 0;
        while (!Incoming().Empty())
        {
            if(processedMessages >= MAX_CLIENT_PROCESS_MESSAGE)
                break;

            auto message = Incoming().Pop();
            Engine::Dispatcher().trigger<Message<Archetype>>(message);

            processedMessages++;
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

    ConcurrentQueue<Message<Archetype>> m_messageInput;
};