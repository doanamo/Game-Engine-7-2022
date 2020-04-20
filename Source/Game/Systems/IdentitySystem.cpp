/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#include "Game/Systems/IdentitySystem.hpp"
#include "Game/EntitySystem.hpp"
using namespace Game;

IdentitySystem::IdentitySystem()
{
    m_entityDestroyReceiver.Bind<IdentitySystem, &IdentitySystem::OnEntityDestroyed>(this);
}

IdentitySystem::~IdentitySystem() = default;

IdentitySystem::CreateResult IdentitySystem::Create(EntitySystem* entitySystem)
{
    LOG("Creating identity system...");
    LOG_SCOPED_INDENT();

    // Validate arguments.
    CHECK_ARGUMENT_OR_RETURN(entitySystem != nullptr, Common::Failure(CreateErrors::InvalidArgument));

    // Create instance.
    auto instance = std::unique_ptr<IdentitySystem>(new IdentitySystem());

    // Subscribe event receiver.
    instance->m_entityDestroyReceiver.Subscribe(entitySystem->events.entityDestroy);

    // Success!
    return Common::Success(std::move(instance));
}

bool IdentitySystem::SetEntityName(EntityHandle entity, std::string name, bool rename)
{
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
        // Create new entity lookup entry.
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
            // Create new name lookup registry.
            auto nameInsertResult = m_nameLookup.emplace(name, NameRegistry{ entity });
            ASSERT(nameInsertResult.second, "Failed to insert a new entity name lookup registry!");
        }
    }

    return true;
}

EntityHandle IdentitySystem::GetEntityByName(std::string name) const
{
    // Find name in name lookup table.
    auto nameIt = m_nameLookup.find(name);

    if(nameIt != m_nameLookup.end())
    {
        // Retrieve name registry.
        const NameRegistry& nameRegistry = nameIt->second;
        ASSERT(!nameRegistry.empty(), "Retrieved an empty registry name!");

        // Return first named entity.
        return nameRegistry.front();
    }

    return EntityHandle();
}

std::vector<EntityHandle> IdentitySystem::GetEntitiesWithName(std::string name) const
{
    // Find name in name lookup table.
    auto nameIt = m_nameLookup.find(name);

    if(nameIt != m_nameLookup.end())
    {
        // Retrieve name registry.
        const NameRegistry& nameRegistry = nameIt->second;
        ASSERT(!nameRegistry.empty(), "Retrieved an empty registry name!");

        // Return copy of name registry.
        return nameRegistry;
    }

    return NameRegistry();
}

void IdentitySystem::OnEntityDestroyed(EntityHandle entity)
{
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
