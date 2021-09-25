#pragma once

#include "core/core.h"
#include "core/engine.h"

#include "common.h"
#include "message.h"
#include "concurrent_queue.h"

#include <thread>
#include <chrono>
#include <deque>
#include <unordered_set>
// #include <unordered_map>

using namespace dagger;

template <typename Archetype>
class NetworkServer
{
protected:
    struct OwnedMessage
    {
        UInt32 remote;
        Message<Archetype> message;

        friend std::ostream& operator<<(std::ostream& os, const OwnedMessage& message)
        {
            os << message.message;
            return os;
        }
    };
    
    class Connection
    {
    public:
        Connection(asio::io_context& context_, asio::ip::tcp::socket socket_, ConcurrentQueue<OwnedMessage>& messageIn_) 
            : m_context(context_), m_socket(std::move(socket_)), m_messageInput(messageIn_)
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

        void AuthenticateClient(NetworkServer<Archetype>* server_, UInt32 id_ = 0)
        {
            if (IsConnected())
            {
                m_id = id_;

                WriteValidation(server_);
            }
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
            asio::async_read(m_socket, asio::buffer(&m_tempMessageIn.header, sizeof(typename Message<Archetype>::Header)),
            [this](std::error_code error, std::size_t length){
                if(!error)
                {
                    if(m_tempMessageIn.header.Size() > 0)
                    {
                        if (m_tempMessageIn.header.Size() > MAX_MESSAGE_SIZE)
                        {
                            Logger::error("Message Too Large. Closing Connection");
                            m_socket.close();
                        }
                        else
                        {
                            m_tempMessageIn.body.resize(m_tempMessageIn.header.Size());
                            ReadBody();
                        }
                    }
                    else
                    {
                        m_messageInput.Push({this->GetId(), m_tempMessageIn});
                        ReadHeader();
                    }
                }
                else
                {
                    Logger::error("Failed Reading Message Header: {}", m_id);
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
                    m_messageInput.Push({this->GetId(), m_tempMessageIn});
                    ReadHeader();
                }
                else
                {
                    Logger::error("Failed Reading Message Body: {}", m_id);
                    m_socket.close();
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
                    Logger::error("Failed Writing Message Header: {}", m_id);
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
                    Logger::error("Failed Writing Message Body: {}", m_id);
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

        /* async */ void WriteValidation(NetworkServer<Archetype>* server_)
        {
            asio::async_write(m_socket, asio::buffer(&m_handshakeOut, sizeof(UInt64)),
            [this, server_](std::error_code error, std::size_t length){
                if(!error)
                {
                    ReadValidation(server_);
                }
                else
                {
                    Logger::error("Failed Writing Validation Message: {}", m_id);
                    m_socket.close();
                }
            });
        }

        // TODO rm param ?
        /* async */ void ReadValidation(NetworkServer<Archetype>* server_)
        {
            asio::async_read(m_socket, asio::buffer(&m_handshakeIn, sizeof(UInt64)),
            [this, server_](std::error_code error, std::size_t length){
                if(!error)
                {
                    if (m_handshakeIn == m_handshakeCheck)
                    {
                        Logger::info("Connection Validation Approved: {}", m_id);
                        // TODO ?
                        server_->OnClientValidated(this->GetId());

                        ReadHeader();
                    }
                    else
                    {
                        Logger::error("Failed Writing Validation Message: {}", m_id);
                        m_socket.close();
                    }
                }
                else
                {
                    Logger::error("Connection Validation Failed: {}", m_id);
                    m_socket.close();
                }
            });
        }

    private:
        UInt32 m_id;

        asio::io_context& m_context;
        asio::ip::tcp::socket m_socket;

        ConcurrentQueue<Message<Archetype>> m_messageOutput;
        ConcurrentQueue<OwnedMessage>& m_messageInput;
        Message<Archetype> m_tempMessageIn;

        UInt64 m_handshakeOut = 0;
        UInt64 m_handshakeIn = 0;
        UInt64 m_handshakeCheck = 0;
    };

public:
    NetworkServer(UInt16 port)
        : m_acceptor(m_context, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port))
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
                        m_context, std::move(socket), m_messageIn);

                        // TODO remove 'this' param
                        newConnection->AuthenticateClient(this, m_idCounter++);
                        Logger::debug("Connection Approved: {}", newConnection->GetId());

                        // TODO Thread safety
                        m_connections.insert(std::make_pair<UInt32, std::unique_ptr<Connection>>(
                            newConnection->GetId(),
                            std::move(newConnection))
                        );
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
        auto& client = m_connections[clientId_];
        if (client && client->IsConnected())
        {
            client->Send(message_);
        }
        else
        {
            OnClientDisconnect(clientId_);
            client.reset();

            m_connections.erase(clientId_);
        }
    }

    void Broadcast(const Message<Archetype>& message_, UInt32 ignore_ = nullptr) 
    {
        for (auto it = m_connections.begin(); it != m_connections.end();)
        {
            auto& client = it->second;
            if (client && client->IsConnected())
            {
                if (it->first != ignore_)
                    client->Send(message_);
                ++it;
            }
            else
            {
                OnClientDisconnect(it->first);
                client.reset();
                it = m_connections.erase(it);
            }
        }
    }

    void Update(UInt16 maxMessages_ = -1, bool wait_ = false)
    {
        // Block until a message is received
        if(wait_)
            m_messageIn.wait();

        UInt16 messageCount = 0;
        while(messageCount < maxMessages_ && !m_messageIn.Empty())
        {
            auto message = m_messageIn.Pop();
            OnMessage(message.remote, message.message);
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

    // TODO
    // virtual void OnClientConnect(UInt32 client_)
    // {
        
    // }

    virtual void OnClientDisconnect(UInt32 clientId_)
    {

    }

    // TODO Use events
    virtual void OnMessage(UInt32 clientId_, Message<Archetype>& message_)
    {

    }

    virtual void OnClientValidated(UInt32 clientId_)
    {

    }

private:
    ConcurrentQueue<OwnedMessage> m_messageIn;

    std::unordered_map<UInt32, std::unique_ptr<Connection>> m_connections;

    asio::io_context m_context;
    std::thread m_contextThread;

    asio::ip::tcp::acceptor m_acceptor;

    UInt32 m_idCounter = 10000;
};
