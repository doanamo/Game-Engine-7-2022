/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#pragma once

#include <queue>
#include <Common/HandleMap.hpp>
#include <Common/Event/Dispatcher.hpp>
#include <Common/Event/Collector.hpp>
#include "Game/EntityHandle.hpp"

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
        bool IsEntityCreated(const EntityHandle entity) const;
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
}
