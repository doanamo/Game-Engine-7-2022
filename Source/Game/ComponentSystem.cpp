/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#include "Game/Precompiled.hpp"
#include "Game/ComponentSystem.hpp"
#include "Game/EntitySystem.hpp"
#include "Game/GameInstance.hpp"
using namespace Game;

ComponentSystem::ComponentSystem()
{
    m_entityCreate.Bind<ComponentSystem, &ComponentSystem::OnEntityCreate>(this);
    m_entityDestroy.Bind<ComponentSystem, &ComponentSystem::OnEntityDestroy>(this);
}

ComponentSystem::~ComponentSystem() = default;

bool ComponentSystem::OnAttach(const GameSystemStorage& gameSystems)
{
    ASSERT(m_entitySystem == nullptr);

    m_entitySystem = gameSystems.Locate<EntitySystem>();
    if(m_entitySystem == nullptr)
    {
        LOG_ERROR("Could not retrieve entity system!");
        return false;
    }

    if(!m_entityCreate.Subscribe(m_entitySystem->events.entityCreate))
    {
        LOG_ERROR("Failed to subscribe to entity system!");
        return false;
    }

    if(!m_entityDestroy.Subscribe(m_entitySystem->events.entityDestroy))
    {
        LOG_ERROR("Failed to subscribe to entity system!");
        return false;
    }

    return true;
}

bool ComponentSystem::OnEntityCreate(EntityHandle handle)
{
    // Initialize all components belonging to this entity.
    for(auto& pair : m_pools)
    {
        auto& pool = pair.second;

        if(!pool->InitializeComponent(handle))
            return false;
    }

    return true;
}

void ComponentSystem::OnEntityDestroy(EntityHandle handle)
{
    // Remove all components belonging to the destroyed entity from every pool.
    for(auto& pair : m_pools)
    {
        auto& pool = pair.second;
        pool->DestroyComponent(handle);
    }
}

EntitySystem* ComponentSystem::GetEntitySystem() const
{
    return m_entitySystem;
}

const EntityEntry* ComponentSystem::GetEntityEntry(EntityHandle handle) const
{
    return m_entitySystem->GetEntityEntry(handle);
}
