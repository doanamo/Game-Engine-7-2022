/*
    Copyright (c) 2018 Piotr Doan. All rights reserved.
*/

#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include "Events/Receiver.hpp"
#include "Game/EntityHandle.hpp"

/*
    Identity System

    Manages entities and their names.
*/

namespace Game
{
    // Forward declarations.
    class EntitySystem;

    // Identity system class.
    class IdentitySystem
    {
    public:
        // Type declarations.
        using NameRegistry = std::vector<EntityHandle>;
        using EntityLookup = std::unordered_map<EntityHandle, std::string>;
        using NameLookup = std::unordered_map<std::string, NameRegistry>;

    public:
        IdentitySystem();
        ~IdentitySystem();

        // Disable copying.
        IdentitySystem(const IdentitySystem& other) = delete;
        IdentitySystem& operator=(const IdentitySystem& other) = delete;

        // Move constructor and assignment.
        IdentitySystem(IdentitySystem&& other);
        IdentitySystem& operator=(IdentitySystem&& other);

        // Initializes the identity system.
        bool Initialize(EntitySystem& entitySystem);

        // Sets an entity name.
        bool SetEntityName(EntityHandle entity, std::string name, bool rename = true);

        // Gets an entity name.
        // Returns the first entity with this name if more exists.
        EntityHandle GetEntityByName(std::string name) const;

        // Gets all entities with name.
        std::vector<EntityHandle> GetEntitiesWithName(std::string name) const;

    public:
        // Handles an entity being destroyed.
        void OnEntityDestroyed(EntityHandle entity);

    private:
        // Initialization state.
        bool m_initialized;

        // Lookup tables.
        EntityLookup m_entityLookup;
        NameLookup m_nameLookup;

        // Event receiver.
        Common::Receiver<void(EntityHandle)> m_entityDestroyedReceiver;
    };
}
