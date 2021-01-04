/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#include "Game/Precompiled.hpp"
#include "Game/Systems/IdentitySystem.hpp"
#include "Game/EntitySystem.hpp"
using namespace Game;

IdentitySystem::IdentitySystem()
{
    m_entityDestroyReceiver.Bind<IdentitySystem, &IdentitySystem::OnEntityDestroyed>(this);
}

IdentitySystem::~IdentitySystem() = default;

IdentitySystem::CreateResult IdentitySystem::Create(const CreateFromParams& params)
{
    CHECK_ARGUMENT_OR_RETURN(params.entitySystem != nullptr,
        Common::Failure(CreateErrors::InvalidArgument));

    auto instance = std::unique_ptr<IdentitySystem>(new IdentitySystem());
    instance->m_entityDestroyReceiver.Subscribe(params.entitySystem->events.entityDestroy);
    instance->m_entitySystem = params.entitySystem;

    LOG_SUCCESS("Created identity system instance.");
    return Common::Success(std::move(instance));
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

    auto entityNameResult = m_entityNameLookup.emplace(entity, name);
    ASSERT(entityNameResult.second, "Failed to emplace entry in entity name lookup!");

    auto nameEntityResult = m_nameEntityLookup.emplace(name, entity);
    ASSERT(nameEntityResult.second, "Failed to emplace entry in name entity lookup!");
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

    auto entityGroupsResult = entityGroupsIt->second.insert(group);
    ASSERT(entityGroupsResult.second, "Failed to emplace entry in entity groups lookup!");

    auto groupEntitiesResult = groupEntitiesIt->second.insert(entity);
    ASSERT(groupEntitiesResult.second, "Failed to emplace entry in group entities lookup!");
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

            int removedCount = groupEntitiesIt->second.erase(entity);
            ASSERT(removedCount == 1, "Unexpected number of removed entries!");

            if(groupEntitiesIt->second.empty())
            {
                m_groupEntitiesLookup.erase(groupEntitiesIt);
            }
        }

        m_entityGroupsLookup.erase(entityGroupsIt);
    }
}
