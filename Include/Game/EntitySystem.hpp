/*
    Copyright (c) 2018 Piotr Doan. All rights reserved.
*/

#pragma once

#include <queue>
#include "Events/Dispatcher.hpp"
#include "Events/Collector.hpp"
#include "Game/EntityHandle.hpp"

/*
    Entity System

    Manages unique identifiers for each existing entity. Gives means to identify
    different entities and takes care of their safe creation and destruction.

    void ExampleEntitySystem
    {
        // Create an entity system.
        Game::EntitySystem entitySystem;
        entitySystem.Initialize();
    
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

                // Entity handle exists and can be referenced.
                // Systems may not acknowledge this entity yet and its
                // components may be still in uninitialized state.
                Exists = 1 << 0,

                // Entity handle exists and has been officially created.
                // Important difference is that other systems have been
                // informed about an entity being created, resulting in
                // its components being initialized as well.
                Created = 1 << 1,

                // Entity handle has been scheduled to be destroyed.
                Destroy = 1 << 2,
            };

            using Type = unsigned int;
        };

    private:
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
            enum
            {
                Invalid,

                Create,
                Destroy,
            };

            using Type = int;
        };

        // Entity command structure.
        struct EntityCommand
        {
            EntityCommand(EntityCommands::Type type, EntityHandle handle);

            EntityCommands::Type type;
            EntityHandle handle;
        };

        // Type declarations.
        using EntryList = std::vector<HandleEntry>;
        using FreeList = std::queue<std::size_t>;
        using CommandList = std::queue<EntityCommand>;

    public:
        EntitySystem();
        ~EntitySystem();

        // Disallow copying.
        EntitySystem(const EntitySystem& other) = delete;
        EntitySystem& operator=(const EntitySystem& other) = delete;

        // Move constructor and assignment.
        EntitySystem(EntitySystem&& other);
        EntitySystem& operator=(EntitySystem&& other);

        // Initializes the entity system.
        bool Initialize();

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

        // Returns current entity's flags.
        HandleFlags::Type GetEntityFlags(const EntityHandle& entity);

        // Returns the number of active entities.
        unsigned int GetEntityCount() const
        {
            return m_entityCount;
        }

    public:
        // Event that are dispatched on ProcessCommands() call.
        struct Events
        {
            using EntityCreateDispatcher = Common::Dispatcher<bool(EntityHandle), Common::CollectWhileTrue>;
            using EntityDestroyDispatcher = Common::Dispatcher<void(EntityHandle)>;

            EntityCreateDispatcher entityCreate;
            EntityDestroyDispatcher entityDestroy;
        } events;

    private:
        // Returns a handle entry.
        const HandleEntry& GetHandleEntry(const EntityHandle& entity) const;
        HandleEntry& GetHandleEntry(const EntityHandle& entity);

        // Frees an entity handle.
        void FreeHandle(HandleEntry& handleEntry);

    private:
        // List of commands.
        CommandList m_commands;

        // List of entity handles.
        EntryList m_handleEntries;

        // List of freed entity identifiers.
        FreeList m_freeIdentifiers;

        // Number of active entities.
        unsigned int m_entityCount;

        // Initialization state.
        bool m_initialized;
    };
}
