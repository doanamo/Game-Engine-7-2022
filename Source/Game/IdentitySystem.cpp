/*
    Copyright (c) 2018 Piotr Doan. All rights reserved.
*/

#include "Precompiled.hpp"
#include "Game/IdentitySystem.hpp"
#include "Game/EntitySystem.hpp"
using namespace Game;

IdentitySystem::IdentitySystem() :
    m_initialized(false)
{
    // Bind event receivers.
    m_entityDestroyedReceiver.Bind<IdentitySystem, &IdentitySystem::OnEntityDestroyed>(this);
}

IdentitySystem::~IdentitySystem()
{
}

bool IdentitySystem::Initialize(EntitySystem& entitySystem)
{
    LOG() << "Initializing identity system...";

    // Make sure instance is not already initialized.
    ASSERT(!m_initialized, "Identity system instance has already been initialized!");

    // Subscribe event receiver.
    m_entityDestroyedReceiver.Subscribe(entitySystem.events.entityDestroyed);

    // Success!
    return m_initialized = true;;
}

bool IdentitySystem::SetEntityName(EntityHandle entity, std::string name, bool rename)
{
    ASSERT(m_initialized, "Identity system has not been initialized!");

    // Check if provided entity is already named.
    auto entityIt = m_entityLookup.find(entity);

    if(entityIt != m_entityLookup.end())
    {
        // Check if entity already has this name.
        if(entityIt->second == name)
            return true;

        // Check if we want to rename an existing entity.
        if(!rename)
            return false;

        // Find existing name lookup entry.
        auto nameIt = m_nameLookup.find(entityIt->second);
        ASSERT(nameIt != m_nameLookup.end(), "Found entity lookup name without an associated name lookup registry!");

        // Find entity in name registry.
        NameRegistry& nameRegistry = nameIt->second;
        auto namedEntityIt = std::find(nameRegistry.begin(), nameRegistry.end(), entity);
        ASSERT(namedEntityIt != nameRegistry.end(), "Found entity lookup entry without an associated name registry entry!");

        // Remove entity from name registry.
        nameRegistry.erase(namedEntityIt);

        if(nameRegistry.empty())
        {
            // Remove empty name registry.
            m_nameLookup.erase(nameIt);
        }

        // Check if provided name is not empty.
        if(!name.empty())
        {
            // Change name for entity lookup entry.
            entityIt->second = name;

            // Check if new name is already registered.
            auto nameIt = m_nameLookup.find(name);

            if(nameIt != m_nameLookup.end())
            {
                // Update existing name lookup registry.
                NameRegistry& nameRegistry = nameIt->second;
                nameRegistry.emplace_back(entity);
            }
            else
            {
                // Create a new name lookup registry.
                auto nameInsertResult = m_nameLookup.emplace(name, NameRegistry{ entity });
                ASSERT(nameInsertResult.second, "Failed to insert a new entity name lookup registry!");
            }
        }
        else
        {
            // Remove existing entity lookup entry.
            m_entityLookup.erase(entityIt);
        }
    }
    else
    {
        // Create a new entity lookup entry.
        auto entityInsertResult = m_entityLookup.emplace(entity, name);
        ASSERT(entityInsertResult.second, "Failed to insert a new named entity lookup entry!");

        // Check if such name is already registered.
        auto nameIt = m_nameLookup.find(name);

        if(nameIt != m_nameLookup.end())
        {
            // Update existing name lookup registry.
            NameRegistry& nameRegistry = nameIt->second;
            nameRegistry.emplace_back(entity);
        }
        else
        {
            // Create a new name lookup registry.
            auto nameInsertResult = m_nameLookup.emplace(name, NameRegistry{ entity });
            ASSERT(nameInsertResult.second, "Failed to insert a new entity name lookup registry!");
        }
    }

    return true;
}

EntityHandle IdentitySystem::GetEntityByName(std::string name) const
{
    ASSERT(m_initialized, "Identity system has not been initialized!");

    // Find name in name lookup table.
    auto nameIt = m_nameLookup.find(name);

    if(nameIt != m_nameLookup.end())
    {
        // Retrieve the name registry.
        const NameRegistry& nameRegistry = nameIt->second;
        ASSERT(!nameRegistry.empty(), "Retrieved an empty registry name!");

        // Return the first named entity.
        return nameRegistry.front();
    }

    return EntityHandle();
}

std::vector<EntityHandle> IdentitySystem::GetEntitiesWithName(std::string name) const
{
    ASSERT(m_initialized, "Identity system has not been initialized!");

    // Find name in name lookup table.
    auto nameIt = m_nameLookup.find(name);

    if(nameIt != m_nameLookup.end())
    {
        // Retrieve the name registry.
        const NameRegistry& nameRegistry = nameIt->second;
        ASSERT(!nameRegistry.empty(), "Retrieved an empty registry name!");

        // Return a copy of the name registry.
        return nameRegistry;
    }

    return NameRegistry();
}

void IdentitySystem::OnEntityDestroyed(EntityHandle entity)
{
    ASSERT(m_initialized, "Identity system has not been initialized!");

    // Find entity in the entity lookup table.
    auto entityIt = m_entityLookup.find(entity);

    if(entityIt != m_entityLookup.end())
    {
        // Find existing name lookup entry.
        auto nameIt = m_nameLookup.find(entityIt->second);
        ASSERT(nameIt != m_nameLookup.end(), "Found entity lookup name without an associated name lookup registry!");

        // Find entity in name registry.
        NameRegistry& nameRegistry = nameIt->second;
        auto namedEntityIt = std::find(nameRegistry.begin(), nameRegistry.end(), entity);
        ASSERT(namedEntityIt != nameRegistry.end(), "Found entity lookup entry without an associated name registry entry!");

        // Remove entity from name registry.
        nameRegistry.erase(namedEntityIt);

        if(nameRegistry.empty())
        {
            // Remove empty name registry.
            m_nameLookup.erase(nameIt);
        }

        // Remove existing entity lookup entry.
        m_entityLookup.erase(entityIt);
    }
}
