/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#include "Game/ComponentSystem.hpp"
using namespace Game;

ComponentSystem::ComponentSystem()
{
    m_entityCreate.Bind<ComponentSystem, &ComponentSystem::OnEntityCreate>(this);
    m_entityDestroy.Bind<ComponentSystem, &ComponentSystem::OnEntityDestroy>(this);
}

ComponentSystem::~ComponentSystem() = default;

ComponentSystem::InitializeResult ComponentSystem::Initialize(EntitySystem* entitySystem)
{
    LOG("Initializing component system...");
    LOG_SCOPED_INDENT();

    // Setup initialization guard.
    VERIFY(!m_initialized, "Instance has already been initialized!");
    SCOPE_GUARD_IF(!m_initialized, this->Reset());

    // Save entity system reference.
    CHECK_ARGUMENT_OR_RETURN(entitySystem != nullptr, Failure(InitializeErrors::InvalidArgument));

    m_entitySystem = entitySystem;

    // Receive events from the entity system.
    if(!m_entityCreate.Subscribe(entitySystem->events.entityCreate))
    {
        LOG_ERROR("Failed to subscribe to entity system!");
        return Failure(InitializeErrors::FailedEventSubscription);
    }

    if(!m_entityDestroy.Subscribe(entitySystem->events.entityDestroy))
    {
        LOG_ERROR("Failed to subscribe to entity system!");
        return Failure(InitializeErrors::FailedEventSubscription);
    }

    // Success!
    m_initialized = true;
    return Success();
}

bool ComponentSystem::OnEntityCreate(EntityHandle handle)
{
    ASSERT(m_initialized, "Component system has not been initialized!");
    
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
    ASSERT(m_initialized, "Component system has not been initialized");

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
