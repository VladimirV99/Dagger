#pragma once

#include "common.h"

#include <iostream>

#include "core/core.h"
#include "core/engine.h"

using namespace dagger;

template <typename Archetype>
struct MessageHeader
{
    Archetype id {}; // Message Type ID
    UInt32 size = 0; // Size of the body
};

template <typename Archetype>
struct Message
{
    MessageHeader<Archetype> header;
    Sequence<UInt8> body;

    size_t size() const
    {
        return sizeof(MessageHeader<Archetype>) + body.size();
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
        message.header.size = message.body.size();

        return message;
    }

    template <typename Datatype>
    friend Message<Archetype>& operator >> (Message<Archetype>& message, Datatype& data)
    {
        // Check is type deserializable
        static_assert(std::is_standard_layout<Datatype>::value, "Value can't be deserialized");

        // Copy the data
        size_t i = message.body.size() - sizeof(Datatype);
        std::memcpy(&data, message.body.data() + i, sizeof(Datatype));
        message.body.resize(i);

        // Update header size
        message.header.size = message.body.size();

        return message;
    }
};