/*
    Copyright (c) 2018 Piotr Doan. All rights reserved.
*/

#include "Precompiled.hpp"
#include "Game/ComponentSystem.hpp"
#include "Game/EntitySystem.hpp"
using namespace Game;

ComponentSystem::ComponentSystem() :
    m_initialized(false)
{
    // Bind event receiver.
    m_entityDestroyed.Bind<ComponentSystem, &ComponentSystem::OnEntityDestroy>(this);
}

ComponentSystem::~ComponentSystem()
{
}

ComponentSystem::ComponentSystem(ComponentSystem&& other) :
    ComponentSystem()
{
    // Call the move assignment.
    *this = std::move(other);
}

ComponentSystem& ComponentSystem::operator=(ComponentSystem&& other)
{
    // Swap class members.
    std::swap(m_initialized, other.m_initialized);
    std::swap(m_pools, other.m_pools);
    std::swap(m_entityDestroyed, other.m_entityDestroyed);

    return *this;
}

bool ComponentSystem::Initialize(EntitySystem& entitySystem)
{   
    LOG() << "Initializing component system..." << LOG_INDENT();

    // Make sure we are not initializing twice.
    ASSERT(!m_initialized, "Component system instance has already been initialized!");

    // Receive events about destroyed entities.
    if(!m_entityDestroyed.Subscribe(entitySystem.events.entityDestroyed))
    {
        LOG_ERROR() << "Failed to subscribe to entity system!";
        return false;
    }

    // Success!
    return m_initialized = true;
}

void ComponentSystem::OnEntityDestroy(EntityHandle handle)
{
    ASSERT(m_initialized, "Component system instance is not initialized!");

    // Remove all components belonging to the destroyed entity from every pool.
    for(auto& pair : m_pools)
    {
        auto& pool = pair.second;
        pool->DestroyComponent(handle);
    }
}
