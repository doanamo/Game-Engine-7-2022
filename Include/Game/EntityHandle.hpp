/*
    Copyright (c) 2018 Piotr Doan. All rights reserved.
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
    // Entity handle structure.
    struct EntityHandle
    {
        // Type declarations.
        typedef int ValueType;

        // Constructor.
        EntityHandle() :
            identifier(0),
            version(0)
        {
        }

        // Sorting operator.
        bool operator<(const EntityHandle& other) const
        {
            if(identifier < other.identifier)
                return true;

            if(identifier == other.identifier)
                return version < other.version;

            return false;
        }

        // Comparison operators.
        bool operator==(const EntityHandle& other) const
        {
            return identifier == other.identifier && version == other.version;
        }

        bool operator!=(const EntityHandle& other) const
        {
            return identifier != other.identifier || version != other.version;
        }

        // Handle data.
        ValueType identifier;
        ValueType version;
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
            // Use the identifier as a hash.
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
            // We assume std::size_t is 64bit, but it is fine if not.
            return pair.first.identifier * std::numeric_limits<int>::max() + pair.second.identifier;
        }
    };
}
