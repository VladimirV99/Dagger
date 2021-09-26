#pragma once

#include "common.h"

#include <iostream>

#include "core/core.h"
#include "core/engine.h"

using namespace dagger;

template <typename Archetype>
struct Message
{
    struct Header
    {
        Archetype id {}; // Message Type ID
    private:
        UInt32 m_size = 0; // Size of the body

    public:
        Header() = default;

        Header(Archetype id_)
        : id(id_)
        {

        }

        Header(const Header& other_)
        : id(other_.id), m_size(other_.m_size)
        {

        }

        UInt32 Size() const
        {
            return m_size;
        }

        template <typename Datatype>
        friend Message<Archetype>& operator << (Message<Archetype>& message, const Datatype& data);
        
        template <typename Datatype>
        friend Message<Archetype>& operator >> (Message<Archetype>& message, Datatype& data);
    };

    Header header;
    Sequence<UInt8> body;

    Message() = default;

    Message(Archetype id_)
    : header(id_)
    {

    }

    Message(const Message<Archetype>& other_)
    : header(other_.header), body(other_.body)
    {

    }

    size_t Size() const
    {
        return sizeof(Header) + body.size();
    }

    friend std::ostream& operator <<(std::ostream& os, const Message<Archetype>& message)
    {
        os << "id: " << int(message.header.id) << " size: " << message.header.size;
        return os;
    }

    template <typename Datatype>
    friend Message<Archetype>& operator << (Message<Archetype>& message, const Datatype& data)
    {
        // Check is type serializable
        static_assert(std::is_standard_layout<Datatype>::value, "Value can't be serialized");

        // Copy the data
        size_t i = message.body.size();
        message.body.resize(message.body.size() + sizeof(Datatype));
        std::memcpy(message.body.data() + i, &data, sizeof(Datatype));

        // Update header size
        message.header.m_size = message.body.size();

        return message;
    }

    template <typename Datatype>
    friend Message<Archetype>& operator >> (Message<Archetype>& message, Datatype& data)
    {
        // Check is type deserializable
        static_assert(std::is_standard_layout<Datatype>::value, "Value can't be deserialized");

        if (message.body.size() < sizeof(Datatype))
        {
            Logger::error("Error extracting type from message. Insufficient message size");
            return message;
        }

        // Copy the data
        size_t i = message.body.size() - sizeof(Datatype);
        std::memcpy(&data, message.body.data() + i, sizeof(Datatype));
        message.body.resize(i);

        // Update header size
        message.header.m_size = message.body.size();

        return message;
    }
};

template <typename Archetype>
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