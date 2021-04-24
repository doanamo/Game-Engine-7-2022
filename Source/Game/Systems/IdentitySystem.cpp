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

bool IdentitySystem::OnAttach(GameInstance* gameInstance)
{
    ASSERT(m_entitySystem == nullptr);

    m_entitySystem = gameInstance->GetSystem<EntitySystem>();
    if(m_entitySystem == nullptr)
    {
        LOG_ERROR("Could not retrieve entity system!");
        return false;
    }

    if(!m_entityDestroyReceiver.Subscribe(m_entitySystem->events.entityDestroy))
    {
        LOG_ERROR("Failed to subscribe to entity system!");
        return false;
    }

    return true;
}

void IdentitySystem::OnEntityDestroyed(EntityHandle entity)
{
    UnregisterNamedEntity(entity);
    UnregisterGroupedEntity(entity);
}

IdentitySystem::NamingResult IdentitySystem::SetEntityName(
    EntityHandle entity, std::string name, bool force)
{
    CHECK_ARGUMENT_OR_RETURN(m_entitySystem->IsEntityValid(entity),
        Common::Failure(NamingErrors::InvalidEntity));

    if(!name.empty())
    {
        auto nameEntityIt = m_nameEntityLookup.find(name);
        if(nameEntityIt != m_nameEntityLookup.end())
        {
            if(nameEntityIt->second == entity)
                return Common::Success();

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

    auto entityNameIt = m_entityNameLookup.find(entity);
    if(entityNameIt != m_entityNameLookup.end())
    {
        ASSERT(entityNameIt->second != name);
        UnregisterNamedEntity(entityNameIt);
    }

    RegisterNamedEntity(entity, name);
    return Common::Success();
}

IdentitySystem::LookupResult<EntityHandle>
    IdentitySystem::GetEntityByName(std::string name) const
{
    auto nameEntityIt = m_nameEntityLookup.find(name);
    if(nameEntityIt == m_nameEntityLookup.end())
        return Common::Failure(LookupErrors::NameNotFound);

    return Common::Success(nameEntityIt->second);
}

IdentitySystem::LookupResult<std::string>
    IdentitySystem::GetEntityName(EntityHandle entity) const
{
    CHECK_ARGUMENT_OR_RETURN(m_entitySystem->IsEntityValid(entity),
        Common::Failure(LookupErrors::InvalidEntity));

    auto entityNameIt = m_entityNameLookup.find(entity);
    if(entityNameIt == m_entityNameLookup.end())
        return Common::Failure(LookupErrors::NameNotFound);
     
    return Common::Success(entityNameIt->second);
}

unsigned int IdentitySystem::GetNamedEntityCount() const
{
    ASSERT(m_entityNameLookup.size() == m_nameEntityLookup.size());
    return m_entityNameLookup.size();
}

void IdentitySystem::RegisterNamedEntity(const EntityHandle& entity, const std::string& name)
{
    if(name.empty())
        return;

    ASSERT_EVALUATE(m_entityNameLookup.emplace(entity, name).second,
        "Failed to emplace entry in entity name lookup!");

    ASSERT_EVALUATE(m_nameEntityLookup.emplace(name, entity).second,
        "Failed to emplace entry in name entity lookup!");
}

void IdentitySystem::UnregisterNamedEntity(const EntityNameLookup::iterator entityNameIt)
{
    ASSERT(entityNameIt != m_entityNameLookup.end(), "Expected valid entity iterator!");

    auto nameEntityIt = m_nameEntityLookup.find(entityNameIt->second);
    ASSERT(nameEntityIt != m_nameEntityLookup.end(), "Lookup mismatch between entities and names!");

    m_entityNameLookup.erase(entityNameIt);
    m_nameEntityLookup.erase(nameEntityIt);
}

void IdentitySystem::UnregisterNamedEntity(const EntityHandle& entity)
{
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

    CHECK_ARGUMENT_OR_RETURN(!group.empty(),
        Common::Failure(GroupingErrors::InvalidGroup));

    auto entityGroupsIt = m_entityGroupsLookup.find(entity);
    if(entityGroupsIt != m_entityGroupsLookup.end())
    {
        auto groupIt = entityGroupsIt->second.find(group);
        if(groupIt != entityGroupsIt->second.end())
            return Common::Success();
    }

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

    UnregisterGroupedEntity(entity, group);
    return Common::Success();
}

IdentitySystem::LookupResult<IdentitySystem::GroupEntitiesSet>
    IdentitySystem::GetEntitiesByGroup(std::string group) const
{
    auto groupEntitiesIt = m_groupEntitiesLookup.find(group);
    if(groupEntitiesIt == m_groupEntitiesLookup.end())
        return Common::Failure(LookupErrors::GroupNotFound);

    return Common::Success(groupEntitiesIt->second);
}

IdentitySystem::LookupResult<IdentitySystem::EntityGroupsSet>
    IdentitySystem::GetEntityGroups(EntityHandle entity) const
{
    CHECK_ARGUMENT_OR_RETURN(m_entitySystem->IsEntityValid(entity),
        Common::Failure(LookupErrors::InvalidEntity));

    auto entityGroupsIt = m_entityGroupsLookup.find(entity);
    if(entityGroupsIt == m_entityGroupsLookup.end())
        return Common::Failure(LookupErrors::EntityNotFound);

    return Common::Success(entityGroupsIt->second);
}

bool IdentitySystem::IsEntityInGroup(EntityHandle entity, std::string group) const
{
    auto entityGroupsIt = m_entityGroupsLookup.find(entity);
    if(entityGroupsIt == m_entityGroupsLookup.end())
        return false;

    auto groupResultIt = entityGroupsIt->second.find(group);
    if(groupResultIt == entityGroupsIt->second.end())
        return false;

    return true;
}

unsigned int IdentitySystem::GetGroupedEntityCount() const
{
    return m_entityGroupsLookup.size();
}

unsigned int IdentitySystem::GetGroupCount() const
{
    return m_groupEntitiesLookup.size();
}

void IdentitySystem::RegisterGroupedEntity(const EntityHandle& entity, const std::string& group)
{
    if(group.empty())
        return;

    auto entityGroupsIt = m_entityGroupsLookup.try_emplace(entity).first;
    auto groupEntitiesIt = m_groupEntitiesLookup.try_emplace(group).first;

    ASSERT_EVALUATE(entityGroupsIt->second.insert(group).second,
        "Failed to emplace entry in entity groups lookup!");

    ASSERT_EVALUATE(groupEntitiesIt->second.insert(entity).second,
        "Failed to emplace entry in group entities lookup!");
}

void IdentitySystem::UnregisterGroupedEntity(const EntityHandle& entity, const std::string& group)
{
    auto entityGroupsIt = m_entityGroupsLookup.find(entity);
    if(entityGroupsIt != m_entityGroupsLookup.end())
    {
        entityGroupsIt->second.erase(group);
        if(entityGroupsIt->second.empty())
        {
            m_entityGroupsLookup.erase(entityGroupsIt);
        }
    }

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
    auto entityGroupsIt = m_entityGroupsLookup.find(entity);
    if(entityGroupsIt != m_entityGroupsLookup.end())
    {
        for(const std::string& group : entityGroupsIt->second)
        {
            auto groupEntitiesIt = m_groupEntitiesLookup.find(group);
            ASSERT(groupEntitiesIt != m_groupEntitiesLookup.end(),
                "Lookup mismatch between entities and groups!");

            ASSERT_EVALUATE(groupEntitiesIt->second.erase(entity) == 1,
                "Unexpected number of removed entries!");

            if(groupEntitiesIt->second.empty())
            {
                m_groupEntitiesLookup.erase(groupEntitiesIt);
            }
        }

        m_entityGroupsLookup.erase(entityGroupsIt);
    }
}
