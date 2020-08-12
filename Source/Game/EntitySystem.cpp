/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#include "Precompiled.hpp"
#include "Game/EntitySystem.hpp"
using namespace Game;

EntitySystem::EntitySystem() = default;
EntitySystem::~EntitySystem()
{
    this->DestroyAllEntities();
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

EntitySystem::EntityHandle EntitySystem::CreateEntity()
{
    // Create new entity entry.
    EntityList::HandleEntryRef handleEntry = m_entities.CreateHandle();
    if(!handleEntry.valid)
    {
        ASSERT(false, "Failed to create valid entity entry!");
        return EntityHandle();
    }

    EntityEntry* entityEntry = handleEntry.object;
    ASSERT(entityEntry != nullptr);

    // Mark entity as existing.
    ASSERT(entityEntry->flags == EntityFlags::Unused);
    entityEntry->flags |= EntityFlags::Exists;

    // Queue command for entity creation.
    EntityCommand command;
    command.type = EntityCommands::Create;
    command.handle = handleEntry.handle;
    m_commands.emplace(command);

    // Return entity handle.
    return handleEntry.handle;
}

void EntitySystem::DestroyEntity(const EntityHandle entity)
{
    // Retrieve entity entry.
    EntityList::HandleEntryRef handleEntry = m_entities.LookupHandle(entity);
    if(!handleEntry.valid)
        return;

    EntityEntry* entityEntry = handleEntry.object;
    ASSERT(entityEntry != nullptr);

    // Set handle destroy flag.
    // Also check if entity is already marked for destruction.
    if(entityEntry->flags & EntityFlags::Destroy)
        return;

    entityEntry->flags |= EntityFlags::Destroy;

    // Queue destroy entity command.
    EntityCommand command;
    command.type = EntityCommands::Destroy;
    command.handle = handleEntry.handle;
    m_commands.emplace(command);
}

void EntitySystem::DestroyAllEntities()
{
    // Process outstanding entity commands.
    ProcessCommands();

    // Schedule every entity for destroy.
    for(EntityList::HandleEntryRef handleEntry : m_entities)
    {
        if(handleEntry.object->flags & EntityFlags::Exists)
        {
            DestroyEntity(handleEntry.handle);
        }
    }

    // Process all queued destroy commands.
    ProcessCommands();

    // Check remaining handle count.
    ASSERT(m_entities.GetValidHandleCount() == 0, "Failed to destroy all entity handles!");
}

void EntitySystem::ProcessCommands()
{
    // Guard against infinite loops that can be caused when entity triggers
    // additional commands that in turn could trigger more and so on.
    int iterationCount = 0;

    // Process entity commands.
    while(!m_commands.empty())
    {
        // Check iteration count.
        ASSERT(iterationCount <= 100, "Infinite loop detected! Maximum iteration in entity processing loop has been reached.");

        // Extract and process command list.
        // This is done to operate only on currently queued commands
        // and not ones that could be subsequently queued in the process.
        CommandList commands;
        commands.swap(m_commands);

        while(!commands.empty())
        {
            // Pop command from queue.
            EntityCommand command = commands.front();
            commands.pop();

            // Retrieve entity entry.
            // Handle may no longer be valid and command could be out of date.
            EntityList::HandleEntryRef handleEntry = m_entities.LookupHandle(command.handle);
            if(!handleEntry.valid)
                continue;

            EntityEntry* entityEntry = handleEntry.object;
            ASSERT(entityEntry != nullptr);

            // Process entity command.
            switch(command.type)
            {
            case EntityCommands::Create:
                {
                    // Inform that new entity was created
                    // since last time commands were processed.
                    // This will allow systems to acknowledge this
                    // entity and initialize its components.
                    if(!events.entityCreate(handleEntry.handle))
                    {
                        // Some system failed to initialize this entity.
                        // Destroy the entity immediately and also inform
                        // systems that may have already processed it.
                        events.entityDestroy(handleEntry.handle);
                        m_entities.DestroyHandle(handleEntry.handle);
                        break;
                    }

                    // Mark entity as officially created.
                    ASSERT(entityEntry->flags & EntityFlags::Exists);
                    entityEntry->flags |= EntityFlags::Created;
                }
                break;

            case EntityCommands::Destroy:
                {
                    // Inform about entity being destroyed
                    // since last time commands were processed.
                    events.entityDestroy(handleEntry.handle);

                    // Free the entity handle and return it to the pool.
                    ASSERT(entityEntry->flags & EntityFlags::Destroy);
                    m_entities.DestroyHandle(handleEntry.handle);
                }
                break;
            }
        }

        // Increment iteration count for infinite loop guard.
        ++iterationCount;
    }
}

bool EntitySystem::IsEntityValid(const EntityHandle entity) const
{
    // Retrieve entity entry.
    EntityList::ConstHandleEntryRef handleEntry = m_entities.LookupHandle(entity);
    if(!handleEntry.valid)
        return false;

    const EntityEntry* entityEntry = handleEntry.object;
    ASSERT(entityEntry != nullptr);

    // Make sure queried handle exists.
    ASSERT(entityEntry->flags & EntityFlags::Exists, "Referenced entity handle is not marked as existing!");

    // Make sure entity pointed by queried handle has been created.
    if(!(entityEntry->flags & EntityFlags::Created))
        return false;

    // Check if handle versions match.
    if(handleEntry.handle.GetVersion() != entity.GetVersion())
        return false;

    return true;
}

const EntitySystem::EntityEntry* EntitySystem::GetEntityEntry(const EntityHandle entity) const
{
    // Retrieve handle entry.
    EntityList::ConstHandleEntryRef handleEntry = m_entities.LookupHandle(entity);
    if(!handleEntry.valid)
        return nullptr;

    // Return entity entry.
    const EntityEntry* entityEntry = handleEntry.object;
    ASSERT(entityEntry != nullptr);
    
    return entityEntry;
}

std::size_t EntitySystem::GetEntityCount() const
{
    // Return number of valid handles.
    return m_entities.GetValidHandleCount();
}
