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

    class IdentitySystem : private NonCopyable
    {
    public:
        using NameRegistry = std::vector<EntityHandle>;
        using EntityLookup = std::unordered_map<EntityHandle, std::string>;
        using NameLookup = std::unordered_map<std::string, NameRegistry>;

    public:
        IdentitySystem();
        ~IdentitySystem();

        IdentitySystem(IdentitySystem&& other);
        IdentitySystem& operator=(IdentitySystem&& other);

        bool Initialize(EntitySystem* entitySystem);

        bool SetEntityName(EntityHandle entity, std::string name, bool rename = true);
        EntityHandle GetEntityByName(std::string name) const;
        std::vector<EntityHandle> GetEntitiesWithName(std::string name) const;

    private:
        Event::Receiver<void(EntityHandle)> m_entityDestroyReceiver;
        void OnEntityDestroyed(EntityHandle entity);

    private:
        EntityLookup m_entityLookup;
        NameLookup m_nameLookup;
        
        bool m_initialized = false;
    };
}
