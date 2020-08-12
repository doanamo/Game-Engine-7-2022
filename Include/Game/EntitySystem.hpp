/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#pragma once

#include <queue>
#include <Common/HandleMap.hpp>
#include <Event/Dispatcher.hpp>
#include <Event/Collector.hpp>

/*
    Entity System

    Manages unique identifiers for each existing entity. Gives means to identify
    different entities and takes care of their safe creation and destruction.
*/

namespace Game
{
    class EntitySystem final : private Common::NonCopyable
    {
    public:
        struct EntityFlags
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

            using Type = unsigned char;
        };

        struct EntityEntry
        {
            EntityFlags::Type flags = EntityFlags::Unused;
        };

        using EntityHandle = Common::Handle<EntityEntry>;
        using EntityList = Common::HandleMap<EntityEntry>;

        struct EntityCommands
        {
            enum
            {
                Invalid,
                Create,
                Destroy,
            };

            using Type = unsigned char;
        };

        struct EntityCommand
        {
            EntityHandle handle = {};
            EntityCommands::Type type = EntityCommands::Invalid;
        };

        using CommandList = std::queue<EntityCommand>;

        using CreateResult = Common::Result<std::unique_ptr<EntitySystem>, void>;
        static CreateResult Create();

    public:
        ~EntitySystem();

        EntityHandle CreateEntity();
        void DestroyEntity(const EntityHandle entity);
        void DestroyAllEntities();
        void ProcessCommands();

        bool IsEntityValid(const EntityHandle entity) const;
        const EntityEntry* GetEntityEntry(const EntityHandle entity) const;
        std::size_t GetEntityCount() const;

        struct Events
        {
            Event::Dispatcher<bool(EntityHandle), Event::CollectWhileTrue> entityCreate;
            Event::Dispatcher<void(EntityHandle)> entityDestroy;
        } events;

    private:
        EntitySystem();

        CommandList m_commands;
        EntityList m_entities;
    };

    using EntityHandle = typename EntitySystem::EntityHandle;
}
