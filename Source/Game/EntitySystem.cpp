/*
    Copyright (c) 2018 Piotr Doan. All rights reserved.
*/

#include "Precompiled.hpp"
#include "Game/EntitySystem.hpp"
using namespace Game;

EntitySystem::Events::Events()
{
}

EntitySystem::EntitySystem()
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
    // Create a new entity handle with created flag.
    EntityHandle entityHandle = m_entities.Create(HandleFlags::Created);

    // Notify about a created entity.
    EntityCommand command;
    command.type = EntityCommands::Created;
    command.handle = entityHandle;
    m_commands.push(command);

    // Return the entity handle.
    return entityHandle;
}

void EntitySystem::DestroyEntity(const EntityHandle& entityHandle)
{
    // Retrieve entity flags.
    HandleFlags::Type* flags = m_entities.Lookup(entityHandle);

    if(flags == nullptr)
        return;

    // Check if entity has been created.
    if(!(*flags & HandleFlags::Created))
        return;

    // Check if entity is already marked to be destroyed.
    if(*flags & HandleFlags::Destroy)
        return;

    // Set the handle destroy flag.
    *flags |= HandleFlags::Destroy;

    // Add a destroy entity command.
    EntityCommand command;
    command.type = EntityCommands::Destroy;
    command.handle = entityHandle;
    m_commands.push(command);
}

void EntitySystem::DestroyAllEntities()
{
    // Process outstanding entity commands.
    ProcessCommands();

    // Invalidate all handle entries.
    for(auto it = m_entities.Begin(); it != m_entities.End(); ++it)
    {
        DestroyEntity(it.GetHandle());
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
                // Inform that a  new entity was created
                // since last time commands were processed.
                this->events.entityCreated(command.handle);
            }
            break;

            case EntityCommands::Destroy:
            {
                // Inform about an entity being destroyed
                // since last time commands were processed.
                this->events.entityDestroyed(command.handle);

                // Remove entity handle.
                bool result = m_entities.Remove(command.handle);
                ASSERT(result, "Failed to remove a known entity handle!");
            }
            break;
        }

        // Remove command from the queue.
        m_commands.pop();
    }
}

bool EntitySystem::IsHandleValid(const EntityHandle& entityHandle) const
{
    // Retrieve entity flags.
    const HandleFlags::Type* flags = m_entities.Lookup(entityHandle);

    if(flags == nullptr)
        return false;

    // Check if entity has been created.
    if(!(*flags & HandleFlags::Created))
        return false;
    
    return true;
}
