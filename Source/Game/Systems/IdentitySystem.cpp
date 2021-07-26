/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#include "Game/Precompiled.hpp"
#include "Game/Systems/IdentitySystem.hpp"
#include "Game/EntitySystem.hpp"
#include "Game/GameInstance.hpp"
using namespace Game;

IdentitySystem::IdentitySystem()
{
    m_entityDestroyReceiver.Bind<IdentitySystem, &IdentitySystem::OnEntityDestroyed>(this);
}

IdentitySystem::~IdentitySystem() = default;

bool IdentitySystem::OnAttach(const GameSystemStorage& gameSystems)
{
    ASSERT(m_entitySystem == nullptr);

    // Retrieve needed game systems.
    m_entitySystem = gameSystems.Locate<EntitySystem>();
    if(m_entitySystem == nullptr)
    {
        LOG_ERROR("Could not retrieve entity system!");
        return false;
    }

    // Subscribe to entity destroy event which let us known when we should unregister entities.
    if(!m_entityDestroyReceiver.Subscribe(m_entitySystem->events.entityDestroy))
    {
        LOG_ERROR("Failed to subscribe to entity system!");
        return false;
    }

    return true;
}

void IdentitySystem::OnEntityDestroyed(EntityHandle entity)
{
    // Remove entity from registry.
    UnregisterNamedEntity(entity);
    UnregisterGroupedEntity(entity);
}

IdentitySystem::NamingResult IdentitySystem::SetEntityName(
    EntityHandle entity, std::string name, bool force)
{
    CHECK_ARGUMENT_OR_RETURN(m_entitySystem->IsEntityValid(entity),
        Common::Failure(NamingErrors::InvalidEntity));

    // For non-empty name argument, before we set it there may be another entity that is already
    // using it that we need to clear first as names cannot be duplicated.
    if(!name.empty())
    {
        auto nameEntityIt = m_nameEntityLookup.find(name);
        if(nameEntityIt != m_nameEntityLookup.end())
        {
            // Check if we are trying to set a name that target entity already has.
            if(nameEntityIt->second == entity)
                return Common::Success();

            // Stealing name from another entity requires force flag as confirmation.
            if(force)
            {
                auto entityNameIt = m_entityNameLookup.find(nameEntityIt->second);
                ASSERT(entityNameIt != m_entityNameLookup.end());
                UnregisterNamedEntity(entityNameIt);
            }
            else
            {
                return Common::Failure(NamingErrors::ReservedName);
            }
        }
    }

    // Unregister any existing name for target entity.
    auto entityNameIt = m_entityNameLookup.find(entity);
    if(entityNameIt != m_entityNameLookup.end())
    {
        ASSERT(entityNameIt->second != name);
        UnregisterNamedEntity(entityNameIt);
    }

    // Register new name for target entity.
    RegisterNamedEntity(entity, name);
    return Common::Success();
}

IdentitySystem::LookupResult<EntityHandle>
    IdentitySystem::GetEntityByName(std::string name) const
{
    auto nameEntityIt = m_nameEntityLookup.find(name);
    if(nameEntityIt != m_nameEntityLookup.end())
    {
        return Common::Success(nameEntityIt->second);
    }
    else
    {
        return Common::Failure(LookupErrors::NameNotFound);
    }
}

IdentitySystem::LookupResult<std::string>
    IdentitySystem::GetEntityName(EntityHandle entity) const
{
    CHECK_ARGUMENT_OR_RETURN(m_entitySystem->IsEntityValid(entity),
        Common::Failure(LookupErrors::InvalidEntity));

    auto entityNameIt = m_entityNameLookup.find(entity);
    if(entityNameIt != m_entityNameLookup.end())
    {
        return Common::Success(entityNameIt->second);
    }
    else
    {
        return Common::Failure(LookupErrors::NameNotFound);
    }
}

std::size_t IdentitySystem::GetNamedEntityCount() const
{
    ASSERT(m_entityNameLookup.size() == m_nameEntityLookup.size());
    return m_entityNameLookup.size();
}

void IdentitySystem::RegisterNamedEntity(const EntityHandle& entity, const std::string& name)
{
    if(name.empty())
        return;

    // Register entry in both entity/name lookup tables.
    ASSERT_EVALUATE(m_entityNameLookup.emplace(entity, name).second,
        "Failed to emplace entry in entity name lookup!");

    ASSERT_EVALUATE(m_nameEntityLookup.emplace(name, entity).second,
        "Failed to emplace entry in name entity lookup!");
}

void IdentitySystem::UnregisterNamedEntity(const EntityNameLookup::iterator entityNameIt)
{
    // Unregister named entity using one of the entry iterators.
    ASSERT(entityNameIt != m_entityNameLookup.end(), "Expected valid entity iterator!");

    auto nameEntityIt = m_nameEntityLookup.find(entityNameIt->second);
    ASSERT(nameEntityIt != m_nameEntityLookup.end(), "Lookup mismatch between entities and names!");

    m_entityNameLookup.erase(entityNameIt);
    m_nameEntityLookup.erase(nameEntityIt);
}

void IdentitySystem::UnregisterNamedEntity(const EntityHandle& entity)
{
    // Unregister named entity via entity handle.
    auto entityNameIt = m_entityNameLookup.find(entity);
    if(entityNameIt != m_entityNameLookup.end())
    {
        UnregisterNamedEntity(entityNameIt);
    }
}

IdentitySystem::GroupingResult IdentitySystem::SetEntityGroup(
    EntityHandle entity, std::string group)
{
    CHECK_ARGUMENT_OR_RETURN(m_entitySystem->IsEntityValid(entity),
        Common::Failure(GroupingErrors::InvalidEntity));

    // We cannot use empty group as a way to unregister entity, as one entity can belong to
    // multiple groups at the same time.
    CHECK_ARGUMENT_OR_RETURN(!group.empty(),
        Common::Failure(GroupingErrors::InvalidGroup));

    // Check if target entity does not already belong to this group.
    auto entityGroupsIt = m_entityGroupsLookup.find(entity);
    if(entityGroupsIt != m_entityGroupsLookup.end())
    {
        auto groupIt = entityGroupsIt->second.find(group);
        if(groupIt != entityGroupsIt->second.end())
            return Common::Success();
    }

    // Register target entity in group.
    RegisterGroupedEntity(entity, group);
    return Common::Success();
}

IdentitySystem::GroupingResult IdentitySystem::ClearEntityGroup(
    EntityHandle entity, std::string group)
{
    CHECK_ARGUMENT_OR_RETURN(m_entitySystem->IsEntityValid(entity),
        Common::Failure(GroupingErrors::InvalidEntity));

    CHECK_ARGUMENT_OR_RETURN(!group.empty(),
        Common::Failure(GroupingErrors::InvalidGroup));

    // Unregister entity from this group.
    UnregisterGroupedEntity(entity, group);
    return Common::Success();
}

IdentitySystem::LookupResult<IdentitySystem::GroupEntitiesSet>
    IdentitySystem::GetEntitiesByGroup(std::string group) const
{
    auto groupEntitiesIt = m_groupEntitiesLookup.find(group);
    if(groupEntitiesIt != m_groupEntitiesLookup.end())
    {
        return Common::Success(groupEntitiesIt->second);
    }
    else
    {
        return Common::Failure(LookupErrors::GroupNotFound);
    }
}

IdentitySystem::LookupResult<IdentitySystem::EntityGroupsSet>
    IdentitySystem::GetEntityGroups(EntityHandle entity) const
{
    CHECK_ARGUMENT_OR_RETURN(m_entitySystem->IsEntityValid(entity),
        Common::Failure(LookupErrors::InvalidEntity));

    auto entityGroupsIt = m_entityGroupsLookup.find(entity);
    if(entityGroupsIt != m_entityGroupsLookup.end())
    {
        return Common::Success(entityGroupsIt->second);
    }
    else
    {
        return Common::Failure(LookupErrors::EntityNotFound);
    }
}

bool IdentitySystem::IsEntityInGroup(EntityHandle entity, std::string group) const
{
    // Find entity entry using entity handle.
    auto entityGroupsIt = m_entityGroupsLookup.find(entity);
    if(entityGroupsIt == m_entityGroupsLookup.end())
        return false;

    // Check if entry contains reference to this group.
    auto groupResultIt = entityGroupsIt->second.find(group);
    if(groupResultIt == entityGroupsIt->second.end())
        return false;

    return true;
}

std::size_t IdentitySystem::GetEntityGroupCount(EntityHandle entity) const
{
    ASSERT(m_entityNameLookup.size() == m_nameEntityLookup.size());

    auto entityGroupsIt = m_entityGroupsLookup.find(entity);
    if(entityGroupsIt != m_entityGroupsLookup.end())
    {
        return entityGroupsIt->second.size();
    }
    else
    {
        return 0;
    }
}

void IdentitySystem::RegisterGroupedEntity(const EntityHandle& entity, const std::string& group)
{
    if(group.empty())
        return;

    // Emplace entries in both lookup tables if they do not exist yet.
    auto entityGroupsIt = m_entityGroupsLookup.try_emplace(entity).first;
    auto groupEntitiesIt = m_groupEntitiesLookup.try_emplace(group).first;

    // Insert entries for both lookup tables.
    ASSERT_EVALUATE(entityGroupsIt->second.insert(group).second,
        "Failed to emplace entry in entity groups lookup!");

    ASSERT_EVALUATE(groupEntitiesIt->second.insert(entity).second,
        "Failed to emplace entry in group entities lookup!");
}

void IdentitySystem::UnregisterGroupedEntity(const EntityHandle& entity, const std::string& group)
{
    // Unregister group from entity lookup table.
    auto entityGroupsIt = m_entityGroupsLookup.find(entity);
    if(entityGroupsIt != m_entityGroupsLookup.end())
    {
        entityGroupsIt->second.erase(group);
        if(entityGroupsIt->second.empty())
        {
            m_entityGroupsLookup.erase(entityGroupsIt);
        }
    }

    // Unregister entity from group lookup table.
    auto groupEntitiesIt = m_groupEntitiesLookup.find(group);
    if(groupEntitiesIt != m_groupEntitiesLookup.end())
    {
        groupEntitiesIt->second.erase(entity);
        if(groupEntitiesIt->second.empty())
        {
            m_groupEntitiesLookup.erase(groupEntitiesIt);
        }
    }
}

void IdentitySystem::UnregisterGroupedEntity(const EntityHandle& entity)
{
    // Unregister entity from both lookup table, expelling from all groups as well.
    auto entityGroupsIt = m_entityGroupsLookup.find(entity);
    if(entityGroupsIt != m_entityGroupsLookup.end())
    {
        // Iterate over each group that entity belongs to.
        for(const std::string& group : entityGroupsIt->second)
        {
            // Find group entry and remove target entity from it.
            auto groupEntitiesIt = m_groupEntitiesLookup.find(group);
            ASSERT(groupEntitiesIt != m_groupEntitiesLookup.end(),
                "Lookup mismatch between entities and groups!");

            ASSERT_EVALUATE(groupEntitiesIt->second.erase(entity) == 1,
                "Unexpected number of removed entries!");

            // Erase group if it no longer has any elements.
            if(groupEntitiesIt->second.empty())
            {
                m_groupEntitiesLookup.erase(groupEntitiesIt);
            }
        }

        // Remove entity from entity lookup table.
        m_entityGroupsLookup.erase(entityGroupsIt);
    }
}
