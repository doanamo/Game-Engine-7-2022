/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#include "Precompiled.hpp"
#include "Game/ComponentSystem.hpp"
using namespace Game;

ComponentSystem::ComponentSystem()
{
    m_entityCreate.Bind<ComponentSystem, &ComponentSystem::OnEntityCreate>(this);
    m_entityDestroy.Bind<ComponentSystem, &ComponentSystem::OnEntityDestroy>(this);
}

ComponentSystem::~ComponentSystem() = default;

ComponentSystem::CreateResult ComponentSystem::Create(EntitySystem* entitySystem)
{
    LOG("Create component system...");
    LOG_SCOPED_INDENT();

    // Check arguments.
    CHECK_ARGUMENT_OR_RETURN(entitySystem != nullptr, Common::Failure(CreateErrors::InvalidArgument));

    // Create instance.
    auto instance = std::unique_ptr<ComponentSystem>(new ComponentSystem());

    // Receive events from the entity system.
    if(!instance->m_entityCreate.Subscribe(entitySystem->events.entityCreate))
    {
        LOG_ERROR("Failed to subscribe to entity system!");
        return Common::Failure(CreateErrors::FailedEventSubscription);
    }

    if(!instance->m_entityDestroy.Subscribe(entitySystem->events.entityDestroy))
    {
        LOG_ERROR("Failed to subscribe to entity system!");
        return Common::Failure(CreateErrors::FailedEventSubscription);
    }

    // Save entity system reference
    instance->m_entitySystem = entitySystem;

    // Success!
    return Common::Success(std::move(instance));
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
