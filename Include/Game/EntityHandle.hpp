/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#pragma once

/*
    Entity Handle

    References an unique entity in the world. Consists of two integers,
    an identifier and a version. The version counter is increased every time
    the handler's identifier is reused.
*/

namespace Game
{
    struct EntityHandle
    {
        using ValueType = int;

        EntityHandle() = default;
        EntityHandle(ValueType identifier) :
            identifier(identifier),
            version(0)
        {
        }

        ValueType identifier = 0;
        ValueType version = 0;

        bool operator<(const EntityHandle& other) const
        {
            if(identifier < other.identifier)
                return true;

            if(identifier == other.identifier)
                return version < other.version;

            return false;
        }

        bool operator==(const EntityHandle& other) const
        {
            return identifier == other.identifier && version == other.version;
        }

        bool operator!=(const EntityHandle& other) const
        {
            return identifier != other.identifier || version != other.version;
        }
    };
}

namespace std
{
    // Entity handle hash functor.
    template<>
    struct hash<Game::EntityHandle>
    {
        std::size_t operator()(const Game::EntityHandle& handle) const
        {
            // Use identifier as a hash.
            return handle.identifier;
        }
    };

    // Entity handle pair hash functor.
    template<>
    struct hash<std::pair<Game::EntityHandle, Game::EntityHandle>>
    {
        std::size_t operator()(const std::pair<Game::EntityHandle, Game::EntityHandle>& pair) const
        {
            // Use combined identifiers as a hash.
            // This turns two 32bit integers into one that's 64bit.
            // We assume std::size_t is 64bit, but it is fine if it is not.
            return (std::size_t)pair.first.identifier * std::numeric_limits<int>::max() + pair.second.identifier;
        }
    };
}
