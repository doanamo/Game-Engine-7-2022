/*
    Copyright (c) 2018 Piotr Doan. All rights reserved.
*/

#pragma once

#include <queue>
#include "Events/Dispatcher.hpp"
#include "Game/EntityHandle.hpp"

/*
    Entity System

    Manages unique identifiers for each existing entity. Gives means to identify
    different entities and takes care of their safe creation and destruction.

    void ExampleEntitySystem
    {
        // Create an entity system.
        Game::EntitySystem entitySystem;
    
        // Create a new entity.
        EntityHandle entity = entitySystem.CreateEntity();
        {
            // Entity has been created and components can be added.
        }
        entitySystem.ProcessCommands();
    
        // Destroy the created entity
        entitySystem.DestroyEntity(entity);
        {
            // Entity can be referenced until the next ProcessCommands() call.
        }
        entitySystem.ProcessCommands();
    }
*/

namespace Game
{
    // Entity system class.
    class EntitySystem
    {
    public:
        EntitySystem();
        ~EntitySystem();

        // Creates an entity.
        EntityHandle CreateEntity();

        // Destroys an entity.
        void DestroyEntity(const EntityHandle& entity);

        // Destroys all entities.
        void DestroyAllEntities();

        // Process entity commands.
        void ProcessCommands();

        // Checks if an entity handle is valid.
        bool IsHandleValid(const EntityHandle& entity) const;

        // Returns the number of active entities.
        unsigned int GetEntityCount() const
        {
            return m_entityCount;
        }

    public:
        // Event that are dispatched on ProcessCommands() call.
        struct Events
        {
            Events();

            using EntityCreatedDispatcher = Common::Dispatcher<void(EntityHandle)>;
            using EntityDestroyedDispatcher = Common::Dispatcher<void(EntityHandle)>;

            EntityCreatedDispatcher entityCreated;
            EntityDestroyedDispatcher entityDestroyed;
        } events;

    private:
        // Handle flags.
        struct HandleFlags
        {
            enum
            {
                // Entity handle has been allocated but cannot be used.
                Unused = 0,

                // Entity handle has been created and can be used.
                Created = 1 << 0,

                // Entity handle has been scheduled to be destroyed.
                Destroy = 1 << 1,
            };

            using Type = unsigned int;
        };

        // Handle entry structure.
        struct HandleEntry
        {
            HandleEntry(EntityHandle::ValueType identifier);

            EntityHandle handle;
            HandleFlags::Type flags;
        };

        // Entity command types.
        struct EntityCommands
        {
            enum Type
            {
                Invalid,

                Created,
                Destroy,
            };
        };

        // Entity command structure.
        struct EntityCommand
        {
            EntityCommands::Type type;
            EntityHandle handle;
        };

        // Type declarations.
        using EntryList = std::vector<HandleEntry>;
        using FreeList = std::queue<HandleEntry*>;
        using CommandList = std::queue<EntityCommand>;

    private:
        // Frees an entity handle.
        void FreeHandle(int handleIndex, HandleEntry& handleEntry);

    private:
        // Number of active entities.
        unsigned int m_entityCount;
        
        // List of commands.
        CommandList m_commands;

        // List of entity handles.
        EntryList m_handleEntries;

        // List of free entity handles.
        FreeList m_freeHandles;
    };
}
