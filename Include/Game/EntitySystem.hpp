/*
    Copyright (c) 2018 Piotr Doan. All rights reserved.
*/

#pragma once

#include <queue>
#include "Events/Dispatcher.hpp"
#include "Memory/SlotArray.hpp"

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

        // Type declarations.
        using EntityArray = Common::SlotArray<HandleFlags::Type>;
        using EntityHandle = typename EntitySystem::EntityArray::HandleType;
        
    public:
        EntitySystem();
        ~EntitySystem();

        // Creates an entity.
        EntityHandle CreateEntity();

        // Destroys an entity.
        void DestroyEntity(const EntityHandle& entityHandle);

        // Destroys all entities.
        void DestroyAllEntities();

        // Process entity commands.
        void ProcessCommands();

        // Checks if an entity handle is valid.
        bool IsHandleValid(const EntityHandle& entityHandle) const;

        // Returns the number of active entities.
        unsigned int GetEntityCount() const
        {
            return m_entities.GetSize();
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

        using CommandList = std::queue<EntityCommand>;

    private:
        // List of commands.
        CommandList m_commands;

        // List of entity handles.
        EntityArray m_entities;
    };

    // Declare an entity handle in Game namespace.
    using EntityHandle = typename EntitySystem::EntityHandle;
}
