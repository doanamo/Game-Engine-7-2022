/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <Event/Receiver.hpp>
#include "Game/EntityHandle.hpp"

/*
    Identity System

    Manages entities and their names.
*/

namespace Game
{
    class EntitySystem;

    class IdentitySystem final : private Common::NonCopyable
    {
    public:
        using NameRegistry = std::vector<EntityHandle>;
        using EntityLookup = std::unordered_map<EntityHandle, std::string>;
        using NameLookup = std::unordered_map<std::string, NameRegistry>;

        enum class CreateErrors
        {
            InvalidArgument,
        };

        using CreateResult = Common::Result<std::unique_ptr<IdentitySystem>, CreateErrors>;
        static CreateResult Create(EntitySystem* entitySystem);

        enum class LookupErrors
        {
            NotFound,
        };

        using LookupSingleResult = Common::Result<EntityHandle, LookupErrors>;
        using LookupMultipleResult = Common::Result<std::vector<EntityHandle>, LookupErrors>;

    public:
        ~IdentitySystem();

        bool SetEntityName(EntityHandle entity, std::string name, bool rename = true);
        LookupSingleResult GetEntityByName(std::string name) const;
        LookupMultipleResult GetEntitiesByName(std::string name) const;

    private:
        IdentitySystem();

        Event::Receiver<void(EntityHandle)> m_entityDestroyReceiver;
        void OnEntityDestroyed(EntityHandle entity);

    private:
        EntityLookup m_entityLookup;
        NameLookup m_nameLookup;
    };
}
