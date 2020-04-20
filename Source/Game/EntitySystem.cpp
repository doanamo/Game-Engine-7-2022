/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#include "Game/EntitySystem.hpp"
using namespace Game;

namespace
{
    // Constant variables.
    const EntityHandle::ValueType MaximumIdentifier = std::numeric_limits<EntityHandle::ValueType>::max();
    const EntityHandle::ValueType MaximumVersion = std::numeric_limits<EntityHandle::ValueType>::max();
    const EntityHandle::ValueType InvalidIdentifier = 0;
    const EntityHandle::ValueType StartingIdentifier = 1;

    const std::size_t CachedFreeHandles = 32;
}

EntitySystem::HandleEntry::HandleEntry(EntityHandle::ValueType identifier) :
    handle(identifier), flags(HandleFlags::Unused)
{
}

EntitySystem::EntityCommand::EntityCommand(EntityCommands::Type type, EntityHandle handle) :
    type(type), handle(handle)
{
}

EntitySystem::EntitySystem() = default;

EntitySystem::~EntitySystem()
{
    // Destroy all remaining entities.
    this->DestroyAllEntities();

    // Empty the processing queue.
    this->ProcessCommands();
}

EntitySystem::CreateResult EntitySystem::Create()
{
    LOG("Creating entity system...");
    LOG_SCOPED_INDENT();

    // Create instance.
    auto instance = std::unique_ptr<EntitySystem>(new EntitySystem());
    return Common::Success(std::move(instance));
}

EntityHandle EntitySystem::CreateEntity()
{
    // Check if we have reached the numerical limits.
    VERIFY(m_handleEntries.size() != MaximumIdentifier, "Entity identifier limit has been reached!");

    // Check if the next entity entry on the free list is valid.
    while(!m_freeIdentifiers.empty())
    {
        // Retrieve first free handle from the free list.
        std::size_t handleIndex = m_freeIdentifiers.front() - StartingIdentifier;
        HandleEntry& handleEntry = m_handleEntries[handleIndex];

        // Ensure that the handle entry is free.
        ASSERT(handleEntry.flags == HandleFlags::Unused, "Handle on the free list is being used!");

        // Check if entity entry has exhausted its possible versions.
        if(handleEntry.handle.version == MaximumVersion)
        {
            // Do not use this handle anymore.
            // Discarding a handle will waste a tiny amount of memory.
            m_freeIdentifiers.pop();

            // Attempt to find another candidate for a new entity handle.
            continue;
        }

        // Found good candidate or did not find any at all.
        break;
    }

    // Create new handle if the free list queue is not filled.
    // We maintain a cached amount of handles to avoid situations
    // where a single handle would be reused very quickly, resulting
    // in it exhausting all of its possible versions.
    while(m_freeIdentifiers.size() < CachedFreeHandles)
    {
        // Calculate next unused identifier.
        std::size_t calculatedIdentifier = StartingIdentifier + m_handleEntries.size();
        auto identifier = Common::NumericalCast<EntityHandle::ValueType>(calculatedIdentifier);

        // Create new handle entry.
        m_handleEntries.emplace_back(identifier);

        // Add new handle entry to the free list queue.
        ASSERT(identifier - StartingIdentifier == Common::NumericalCast<EntityHandle::ValueType>(m_handleEntries.size()) - 1,
            "Handle index does not match the added entity identifier!");

        m_freeIdentifiers.emplace(identifier);
    }

    // Retrieve unused handle from the identifier free list.
    std::size_t handleIndex = m_freeIdentifiers.front() - StartingIdentifier;
    HandleEntry& handleEntry = m_handleEntries[handleIndex];
    m_freeIdentifiers.pop();

    // Mark handle as existing.
    ASSERT(handleEntry.flags == HandleFlags::Unused);
    handleEntry.flags |= HandleFlags::Exists;

    // Queue command for entity creation.
    m_commands.emplace(EntityCommands::Create, handleEntry.handle);

    // Return entity handle.
    return handleEntry.handle;
}

void EntitySystem::DestroyEntity(const EntityHandle& entity)
{
    // Check if handle is valid.
    if(!IsHandleValid(entity))
        return;

    // Retrieve handle entry.
    HandleEntry& handleEntry = this->GetHandleEntry(entity);

    // Check if entity is already marked to be destroyed.
    if(handleEntry.flags & HandleFlags::Destroy)
        return;

    // Set handle destroy flag.
    handleEntry.flags |= HandleFlags::Destroy;

    // Add destroy entity command.
    m_commands.emplace(EntityCommands::Destroy, handleEntry.handle);
}

void EntitySystem::DestroyAllEntities()
{
    // Process outstanding entity commands.
    ProcessCommands();

    // Invalidate all handle entries.
    for(HandleEntry& handleEntry : m_handleEntries)
    {
        if(handleEntry.flags & HandleFlags::Exists)
        {
            DestroyEntity(handleEntry.handle);
        }
    }

    // Process all queued destroy commands.
    ProcessCommands();
}

void EntitySystem::ProcessCommands()
{
    // Process entity commands.
    while(!m_commands.empty())
    {
        // Get command from the queue.
        EntityCommand& command = m_commands.front();

        // Process entity command.
        switch(command.type)
        {
            case EntityCommands::Create:
            {
                // Retrieve the handle entry.
                HandleEntry& handleEntry = this->GetHandleEntry(command.handle);
                ASSERT(command.handle == handleEntry.handle);

                // Inform that new entity was created
                // since last time commands were processed.
                // This will allow systems to acknowledge this
                // entity and initialize its components.
                if(!this->events.entityCreate(handleEntry.handle))
                {
                    // Some system failed to initialize this entity.
                    // Destroy the entity immediately and also inform
                    // systems that may have already processed it.
                    this->events.entityDestroy(handleEntry.handle);
                    this->FreeHandle(handleEntry);
                    break;
                }

                // Mark entity as officially created.
                ASSERT(handleEntry.flags & HandleFlags::Exists);
                handleEntry.flags |= HandleFlags::Created;

                // Increment the counter of active entities.
                m_entityCount += 1;
            }
            break;

            case EntityCommands::Destroy:
            {
                // Retrieve handle entry.
                HandleEntry& handleEntry = this->GetHandleEntry(command.handle);
                ASSERT(command.handle == handleEntry.handle);

                // Inform about entity being destroyed
                // since last time commands were processed.
                this->events.entityDestroy(handleEntry.handle);

                // Decrement counter of active entities.
                m_entityCount -= 1;

                // Free the entity handle and return it to the pool.
                ASSERT(handleEntry.flags & HandleFlags::Destroy);
                this->FreeHandle(handleEntry);
            }
            break;
        }

        // Remove command from the queue.
        m_commands.pop();
    }
}

bool EntitySystem::IsHandleValid(const EntityHandle& entity) const
{
    // Retrieve handle entry.
    const HandleEntry& handleEntry = this->GetHandleEntry(entity);

    // Make sure queried handle exists.
    ASSERT(handleEntry.flags & HandleFlags::Exists,
        "Queried entity handle is not marked as existing!");

    // Make sure entity pointed by queried handle has been created.
    if(!(handleEntry.flags & HandleFlags::Created))
        return false;

    // Check if handle versions match.
    if(handleEntry.handle.version != entity.version)
        return false;

    return true;
}

EntitySystem::HandleFlags::Type EntitySystem::GetEntityFlags(const EntityHandle& entity)
{
    // Retrieve handle entry.
    const HandleEntry& handleEntry = this->GetHandleEntry(entity);

    // Check if handle versions match.
    if(handleEntry.handle.version != entity.version)
        return HandleFlags::Unused;
    
    // Return handle flags.
    return handleEntry.flags;
}

const EntitySystem::HandleEntry& EntitySystem::GetHandleEntry(const EntityHandle& entity) const
{
    // Calculate entity handle entry index.
    int handleIndex = entity.identifier - StartingIdentifier;

    // Make sure handle index is withing the expected range.
    ASSERT(handleIndex >= 0, "Invalid entity handle identifier!");
    ASSERT(handleIndex < (int)m_handleEntries.size(), "Invalid entity handle identifier!");

    // Retrieve handle entry.
    return m_handleEntries[handleIndex];
}

EntitySystem::HandleEntry& EntitySystem::GetHandleEntry(const EntityHandle& entity)
{
    return const_cast<HandleEntry&>(const_cast<const EntitySystem*>(this)->GetHandleEntry(entity));
}

void EntitySystem::FreeHandle(HandleEntry& handleEntry)
{
    // Make sure that flags are correct.
    // Does not have to be created yet, as creation process may fail.
    ASSERT(handleEntry.flags & HandleFlags::Exists);

    // Increment the handle version to invalidate it.
    handleEntry.handle.version += 1;

    // Mark handle as free.
    handleEntry.flags = HandleFlags::Unused;

    // Add entity identifier to the free list queue.
    m_freeIdentifiers.emplace(handleEntry.handle.identifier);
}
