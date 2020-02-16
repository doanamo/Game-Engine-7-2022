/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#include "Precompiled.hpp"
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

EntitySystem::EntitySystem() :
    m_entityCount(0),
    m_initialized(false)
{
}

EntitySystem::EntitySystem(EntitySystem&& other) :
    EntitySystem()
{
    // Call the move assignment.
    *this = std::move(other);
}

EntitySystem& EntitySystem::operator=(EntitySystem&& other)
{
    // Swap class members.
    std::swap(events, other.events);

    std::swap(m_commands, other.m_commands);
    std::swap(m_handleEntries, other.m_handleEntries);
    std::swap(m_freeIdentifiers, other.m_freeIdentifiers);
    std::swap(m_entityCount, other.m_entityCount);
    
    std::swap(m_initialized, other.m_initialized);

    return *this;
}

EntitySystem::~EntitySystem()
{
    if(m_initialized)
    {
        // Destroy all remaining entities.
        this->DestroyAllEntities();

        // Empty the processing queue.
        this->ProcessCommands();
    }
}

bool EntitySystem::Initialize()
{
    LOG("Initializing entity system...");
    LOG_SCOPED_INDENT();

    // Make sure instance is not already initialized.
    ASSERT(!m_initialized, "Entity system instance has already been initialized!");

    // Success!
    return m_initialized = true;
}

EntityHandle EntitySystem::CreateEntity()
{
    ASSERT(m_initialized, "Entity system has not been initialized!");

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

        // Found a good candidate or did not find any at all.
        break;
    }

    // Create a new handle if the free list queue is not filled.
    // We maintain a cached amount of handles to avoid situations
    // where a single handle would be reused very quickly, resulting
    // in it exhausting all of its possible versions.
    while(m_freeIdentifiers.size() < CachedFreeHandles)
    {
        // Calculate next unused identifier.
        std::size_t calculatedIdentifier = StartingIdentifier + m_handleEntries.size();
        auto identifier = Utility::NumericalCast<EntityHandle::ValueType>(calculatedIdentifier);

        // Create a new handle entry.
        m_handleEntries.emplace_back(identifier);

        // Add new handle entry to the free list queue.
        ASSERT(identifier - StartingIdentifier == Utility::NumericalCast<EntityHandle::ValueType>(m_handleEntries.size()) - 1,
            "Handle index does not match the added entity identifier!");

        m_freeIdentifiers.emplace(identifier);
    }

    // Retrieve an unused handle from the identifier free list.
    std::size_t handleIndex = m_freeIdentifiers.front() - StartingIdentifier;
    HandleEntry& handleEntry = m_handleEntries[handleIndex];
    m_freeIdentifiers.pop();

    // Mark handle as existing.
    ASSERT(handleEntry.flags == HandleFlags::Unused);
    handleEntry.flags |= HandleFlags::Exists;

    // Queue command for entity creation.
    m_commands.emplace(EntityCommands::Create, handleEntry.handle);

    // Return the entity handle.
    return handleEntry.handle;
}

void EntitySystem::DestroyEntity(const EntityHandle& entity)
{
    ASSERT(m_initialized, "Entity system has not been initialized!");

    // Check if the handle is valid.
    if(!IsHandleValid(entity))
        return;

    // Retrieve the handle entry.
    HandleEntry& handleEntry = this->GetHandleEntry(entity);

    // Check if entity is already marked to be destroyed.
    if(handleEntry.flags & HandleFlags::Destroy)
        return;

    // Set the handle destroy flag.
    handleEntry.flags |= HandleFlags::Destroy;

    // Add a destroy entity command.
    m_commands.emplace(EntityCommands::Destroy, handleEntry.handle);
}

void EntitySystem::DestroyAllEntities()
{
    ASSERT(m_initialized, "Entity system has not been initialized!");

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
    ASSERT(m_initialized, "Entity system has not been initialized!");

    // Process entity commands.
    while(!m_commands.empty())
    {
        // Get the command from the queue.
        EntityCommand& command = m_commands.front();

        // Process entity command.
        switch(command.type)
        {
            case EntityCommands::Create:
            {
                // Retrieve the handle entry.
                HandleEntry& handleEntry = this->GetHandleEntry(command.handle);
                ASSERT(command.handle == handleEntry.handle);

                // Inform that a new entity was created
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
                // Retrieve the handle entry.
                HandleEntry& handleEntry = this->GetHandleEntry(command.handle);
                ASSERT(command.handle == handleEntry.handle);

                // Inform about an entity being destroyed
                // since last time commands were processed.
                this->events.entityDestroy(handleEntry.handle);

                // Decrement the counter of active entities.
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
    ASSERT(m_initialized, "Entity system has not been initialized!");

    // Retrieve the handle entry.
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
    ASSERT(m_initialized, "Entity system has not been initialized!");

    // Retrieve the handle entry.
    const HandleEntry& handleEntry = this->GetHandleEntry(entity);

    // Check if handle versions match.
    if(handleEntry.handle.version != entity.version)
        return HandleFlags::Unused;
    
    // Return handle flags.
    return handleEntry.flags;
}

const EntitySystem::HandleEntry& EntitySystem::GetHandleEntry(const EntityHandle& entity) const
{
    ASSERT(m_initialized, "Entity system has not been initialized!");

    // Calculate entity handle entry index.
    int handleIndex = entity.identifier - StartingIdentifier;

    // Make sure handle index is withing the expected range.
    ASSERT(handleIndex >= 0, "Invalid entity handle identifier!");
    ASSERT(handleIndex < (int)m_handleEntries.size(), "Invalid entity handle identifier!");

    // Retrieve the handle entry.
    return m_handleEntries[handleIndex];
}

EntitySystem::HandleEntry& EntitySystem::GetHandleEntry(const EntityHandle& entity)
{
    ASSERT(m_initialized, "Entity system has not been initialized!");

    return const_cast<HandleEntry&>(const_cast<const EntitySystem*>(this)->GetHandleEntry(entity));
}

void EntitySystem::FreeHandle(HandleEntry& handleEntry)
{
    ASSERT(m_initialized, "Entity system has not been initialized!");

    // Make sure that flags are correct.
    // Does not have to be created yet, as creation process may fail.
    ASSERT(handleEntry.flags & HandleFlags::Exists);

    // Increment the handle version to invalidate it.
    handleEntry.handle.version += 1;

    // Mark the handle as free.
    handleEntry.flags = HandleFlags::Unused;

    // Add the entity identifier to the free list queue.
    m_freeIdentifiers.emplace(handleEntry.handle.identifier);
}
