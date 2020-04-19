/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#pragma once

#include <queue>
#include <vector>
#include "Event/Dispatcher.hpp"
#include "Event/Collector.hpp"
#include "Game/EntityHandle.hpp"

/*
    Entity System

    Manages unique identifiers for each existing entity. Gives means to identify
    different entities and takes care of their safe creation and destruction.
*/

namespace Game
{
    class EntitySystem final : private NonCopyable
    {
    public:
        using CreateResult = Result<std::unique_ptr<EntitySystem>, void>;
        static CreateResult Create();

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

        struct HandleEntry
        {
            HandleEntry(EntityHandle::ValueType identifier);

            EntityHandle handle;
            HandleFlags::Type flags;
        };

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

        struct EntityCommand
        {
            EntityCommand(EntityCommands::Type type, EntityHandle handle);

            EntityCommands::Type type;
            EntityHandle handle;
        };

        using EntryList = std::vector<HandleEntry>;
        using FreeList = std::queue<std::size_t>;
        using CommandList = std::queue<EntityCommand>;

    public:
        ~EntitySystem();

        EntityHandle CreateEntity();
        void DestroyEntity(const EntityHandle& entity);
        void DestroyAllEntities();
        void ProcessCommands();

        bool IsHandleValid(const EntityHandle& entity) const;
        HandleFlags::Type GetEntityFlags(const EntityHandle& entity);

        unsigned int GetEntityCount() const
        {
            return m_entityCount;
        }

    public:
        struct Events
        {
            using EntityCreateDispatcher = Event::Dispatcher<bool(EntityHandle), Event::CollectWhileTrue>;
            using EntityDestroyDispatcher = Event::Dispatcher<void(EntityHandle)>;

            EntityCreateDispatcher entityCreate;
            EntityDestroyDispatcher entityDestroy;
        } events;

    private:
        EntitySystem();

        const HandleEntry& GetHandleEntry(const EntityHandle& entity) const;
        HandleEntry& GetHandleEntry(const EntityHandle& entity);
        void FreeHandle(HandleEntry& handleEntry);

    private:
        CommandList m_commands;
        EntryList m_handleEntries;
        FreeList m_freeIdentifiers;
        unsigned int m_entityCount = 0;
    };
}
