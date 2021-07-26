/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#pragma once

#include <queue>
#include <Common/HandleMap.hpp>
#include <Common/Event/EventDispatcher.hpp>
#include "Game/GameSystem.hpp"
#include "Game/EntityHandle.hpp"

/*
    Entity System

    Manages unique identifiers for each existing entity. Gives means to identify
    different entities and takes care of their safe creation and destruction.
*/

namespace Game
{
    class EntitySystem final : public GameSystem
    {
        REFLECTION_ENABLE(EntitySystem, GameSystem)

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

        using CreateEntityResult = Common::Result<EntityHandle, void>;
        using LookupEntityEntryResult = Common::Result<const EntityEntry*, void>;

    public:
        EntitySystem();
        ~EntitySystem() override;

        void ProcessCommands();

        CreateEntityResult CreateEntity();
        LookupEntityEntryResult LookupEntityEntry(const EntityHandle entity) const;

        void DestroyEntity(const EntityHandle entity);
        void DestroyAllEntities();

        bool IsEntityValid(const EntityHandle entity) const;
        bool IsEntityCreated(const EntityHandle entity) const;

        std::size_t GetEntityCount() const
        {
            return m_entities.GetValidHandleCount();
        }

    public:
        struct Events
        {
            Events();

            Event::Dispatcher<bool(EntityHandle)> entityCreate;
            Event::Dispatcher<void(EntityHandle)> entityDestroy;
        } events;

    private:
        void OnTick(float timeDelta) override;

    private:
        CommandList m_commands;
        EntityList m_entities;
    };
}

REFLECTION_TYPE(Game::EntitySystem, Game::GameSystem)
