/*
    Copyright (c) 2018 Piotr Doan. All rights reserved.
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
}

EntitySystem::Events::Events()
{
}

EntitySystem::HandleEntry::HandleEntry(EntityHandle::ValueType identifier) :
    handle(),
    flags(HandleFlags::Unused),
    nextFree(InvalidIdentifier)
{
    handle.identifier = identifier;
}

EntitySystem::EntitySystem() :
    m_entityCount(0),
    m_freeListDequeue(InvalidIdentifier),
    m_freeListEnqueue(InvalidIdentifier)
{
}

EntitySystem::~EntitySystem()
{
    // Destroy all remaining entities.
    this->DestroyAllEntities();

    // Empty the processing queue.
    this->ProcessCommands();
}

EntityHandle EntitySystem::CreateEntity()
{
    // Check if we have reached the numerical limits.
    VERIFY(m_handles.size() != MaximumIdentifier, "Entity identifier limit has been reached!");

    // Check if the next entity entry on the free list is valid.
    while(m_freeListDequeue != InvalidIdentifier)
    {
        // Retrieve first free handle from the free list.
        int handleIndex = m_freeListDequeue - StartingIdentifier;
        HandleEntry& handleEntry = m_handles[handleIndex];

        // Ensure that the handle entry is free.
        ASSERT(handleEntry.flags == HandleFlags::Unused, "Handle on the free list is being used!");

        // Check if entity entry has exhausted its possible versions.
        if(handleEntry.handle.version == MaximumVersion)
        {
            // Remove handle entry from the free list.
            if(m_freeListDequeue == m_freeListEnqueue)
            {
                // If there was only one element in the queue,
                // set the free list queue state to empty.
                m_freeListDequeue = InvalidIdentifier;
                m_freeListEnqueue = InvalidIdentifier;
            }
            else
            {
                // If there were more than a single element in the queue,
                // set the beginning of the queue to the next free element.
                m_freeListDequeue = handleEntry.nextFree;
            }

            // Clear next free identifier from the exhausted handle entry.
            handleEntry.nextFree = InvalidIdentifier;

            // Attempt to find another candidate for a new entity handle.
            continue;
        }

        // Found a good candidate for a new entity handle.
        break;
    }

    // Create a new handle if the free list queue is empty.
    if(m_freeListDequeue == InvalidIdentifier)
    {
        // Calculate next unused identifier.
        EntityHandle::ValueType identifier = StartingIdentifier + m_handles.size();

        // Create a new handle entry.
        m_handles.emplace_back(identifier);

        // Add new handle entry to the free list queue.
        m_freeListDequeue = identifier;
        m_freeListEnqueue = identifier;
    }

    // Retrieve an unused handle from the free list.
    int handleIndex = m_freeListDequeue - StartingIdentifier;
    HandleEntry& handleEntry = m_handles[handleIndex];

    // Remove unused handle from the free list.
    if(m_freeListDequeue == m_freeListEnqueue)
    {
        // If there was only one element in the queue,
        // set the free list queue state to empty.
        m_freeListDequeue = InvalidIdentifier;
        m_freeListEnqueue = InvalidIdentifier;
    }
    else
    {
        // If there were more than a single element in the queue,
        // set the beginning of the queue to the next free element.
        ASSERT(handleEntry.nextFree != InvalidIdentifier, "Handle entry is missing next free identifier!");

        m_freeListDequeue = handleEntry.nextFree;
        handleEntry.nextFree = InvalidIdentifier;
    }

    // Mark handle as created.
    handleEntry.flags |= HandleFlags::Created;

    // Notify about a created entity.
    EntityCommand command;
    command.type = EntityCommands::Created;
    command.handle = handleEntry.handle;
    m_commands.push(command);

    // Return the entity handle.
    return handleEntry.handle;
}

void EntitySystem::DestroyEntity(const EntityHandle& entity)
{
    // Check if the handle is valid.
    if(!IsHandleValid(entity))
        return;

    // Locate the handle entry.
    int handleIndex = entity.identifier - StartingIdentifier;
    HandleEntry& handleEntry = m_handles[handleIndex];

    // Check if entity is already marked to be destroyed.
    if(handleEntry.flags & HandleFlags::Destroy)
        return;

    // Set the handle destroy flag.
    handleEntry.flags |= HandleFlags::Destroy;

    // Add a destroy entity command.
    EntityCommand command;
    command.type = EntityCommands::Destroy;
    command.handle = handleEntry.handle;
    m_commands.push(command);
}

void EntitySystem::DestroyAllEntities()
{
    // Process outstanding entity commands.
    ProcessCommands();

    // Invalidate all handle entries.
    for(auto it = m_handles.begin(); it != m_handles.end(); ++it)
    {
        HandleEntry& handleEntry = *it;

        if(handleEntry.flags & HandleFlags::Created)
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
        // Get the command from the queue.
        EntityCommand& command = m_commands.front();

        // Process entity command.
        switch(command.type)
        {
            case EntityCommands::Created:
            {
                // Locate the handle entry.
                int handleIndex = command.handle.identifier - StartingIdentifier;
                HandleEntry& handleEntry = m_handles[handleIndex];
                ASSERT(command.handle == handleEntry.handle);

                // Inform that a  new entity was created
                // since last time commands were processed.
                this->events.entityCreated(handleEntry.handle);

                // Increment the counter of active entities.
                m_entityCount += 1;
            }
            break;

            case EntityCommands::Destroy:
            {
                // Locate the handle entry.
                int handleIndex = command.handle.identifier - StartingIdentifier;
                HandleEntry& handleEntry = m_handles[handleIndex];
                ASSERT(command.handle == handleEntry.handle);

                // Inform about an entity being destroyed
                // since last time commands were processed.
                this->events.entityDestroyed(handleEntry.handle);

                // Decrement the counter of active entities.
                m_entityCount -= 1;

                // Free the entity handle and return it to the pool.
                ASSERT(handleEntry.flags & HandleFlags::Destroy);
                this->FreeHandle(handleIndex, handleEntry);
            }
            break;
        }

        // Remove command from the queue.
        m_commands.pop();
    }
}

void EntitySystem::FreeHandle(int handleIndex, HandleEntry& handleEntry)
{
    // Make sure we got the matching index.
    ASSERT(&m_handles[handleIndex] == &handleEntry);

    // Make sure that flags are correct.
    ASSERT(handleEntry.flags & HandleFlags::Created);

    // Increment the handle version to invalidate it.
    handleEntry.handle.version += 1;

    // Mark the handle as free.
    handleEntry.flags = HandleFlags::Unused;

    // Add the handle entry to the free list queue.
    if(m_freeListDequeue == InvalidIdentifier)
    {
        // If there are no elements in the queue,
        // set the element as the only one in the queue.
        m_freeListDequeue = handleEntry.handle.identifier;
        m_freeListEnqueue = handleEntry.handle.identifier;
    }
    else
    {
        // Retrieve last free handle entry on the free list.
        int freeHandleIndex = m_freeListEnqueue - StartingIdentifier;
        HandleEntry& freeHandleEntry = m_handles[freeHandleIndex];

        // Make sure the last element in the queue does not point at another free element.
        ASSERT(freeHandleEntry.nextFree == InvalidIdentifier);

        // If there are other elements in the queue,
        // add the element to the end of the queue chain.
        freeHandleEntry.nextFree = handleEntry.handle.identifier;
        m_freeListEnqueue = handleEntry.handle.identifier;
    }
}

bool EntitySystem::IsHandleValid(const EntityHandle& entity) const
{
    // Check if the handle identifier is valid.
    int handleIndex = entity.identifier - StartingIdentifier;

    if(handleIndex < 0)
        return false;

    // Entity identifiers should never be beyond the current range.
    VERIFY(handleIndex < (int)m_handles.size(), "Invalid handle identifier!")

    // Retrieve the handle entry.
    const HandleEntry& handleEntry = m_handles[handleIndex];

    // Check if handle versions match.
    if(handleEntry.handle.version != entity.version)
        return false;

    // Check if handle is valid.
    if(!(handleEntry.flags & HandleFlags::Created))
        return false;
    
    return true;
}
