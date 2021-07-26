/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#include "Game/Precompiled.hpp"
#include "Game/EntitySystem.hpp"
using namespace Game;

EntitySystem::Events::Events() :
    entityCreate(std::make_unique<Event::CollectWhileTrue>(true))
{
}

EntitySystem::EntitySystem() = default;

EntitySystem::~EntitySystem()
{
    DestroyAllEntities();
    ProcessCommands();
}

void EntitySystem::OnTick(float timeDelta)
{
    ProcessCommands();
}

EntitySystem::CreateEntityResult EntitySystem::CreateEntity()
{
    if(auto cratedHandleResult = m_entities.CreateHandle())
    {
        // Retrieve storage from handle map.
        EntityList::HandleEntryRef handleEntry = cratedHandleResult.Unwrap();
        EntityEntry* entityEntry = handleEntry.GetStorage();
        ASSERT(entityEntry != nullptr);

        // Mark entity as existing.
        ASSERT(entityEntry->flags == EntityFlags::Unused);
        entityEntry->flags |= EntityFlags::Exists;

        // Queue command for entity creation.
        EntityCommand command;
        command.type = EntityCommands::Create;
        command.handle = handleEntry.GetHandle();
        m_commands.emplace(command);

        return Common::Success(handleEntry.GetHandle());
    }
    else
    {
        ASSERT(false, "Failed to create valid entity entry!");
        return Common::Failure();
    }
}

EntitySystem::LookupEntityEntryResult
    EntitySystem::LookupEntityEntry(const EntityHandle entity) const
{
    if(auto lookupHandleResult = m_entities.LookupHandle(entity))
    {
        EntityList::ConstHandleEntryRef handleEntry = lookupHandleResult.Unwrap();
        const EntityEntry* entityEntry = handleEntry.GetStorage();
        ASSERT(entityEntry != nullptr && entityEntry->flags & EntityFlags::Exists);
        return Common::Success(entityEntry);
    }
    else
    {
        return Common::Failure();
    }
}

void EntitySystem::DestroyEntity(const EntityHandle entity)
{
    if(auto lookupHandleResult = m_entities.LookupHandle(entity))
    {
        // Retrieve storage from handle map.
        EntityList::HandleEntryRef handleEntry = lookupHandleResult.Unwrap();
        EntityEntry* entityEntry = handleEntry.GetStorage();
        ASSERT(entityEntry != nullptr);

        // Check if entity is already marked for destruction.
        if(entityEntry->flags & EntityFlags::Destroy)
            return;

        // Mark handle entry for destruction.
        entityEntry->flags |= EntityFlags::Destroy;

        // Queue destroy entity command
        EntityCommand command;
        command.type = EntityCommands::Destroy;
        command.handle = handleEntry.GetHandle();
        m_commands.emplace(command);
    }
}

void EntitySystem::DestroyAllEntities()
{
    // Process outstanding entity commands.
    ProcessCommands();

    // Schedule every entity for destroy.
    for(const EntityList::HandleEntryRef& handleEntry : m_entities)
    {
        ASSERT(handleEntry.GetStorage(), "Entity handle is missing storage!");
        if(handleEntry.GetStorage()->flags & EntityFlags::Exists)
        {
            DestroyEntity(handleEntry.GetHandle());
        }
    }

    // Process all queued destroy commands.
    ProcessCommands();

    // Check remaining handle count.
    ASSERT(m_entities.GetValidHandleCount() == 0, "Failed to destroy all entity handles!");
}

void EntitySystem::ProcessCommands()
{
    // Guard against infinite loops that can be caused when entity triggers additional commands
    // such as create new entities that in turn could trigger more and so on.
    int iterationCount = 0;

    // Process entity commands.
    while(!m_commands.empty())
    {
        // Check iteration count to prevent infinite loops.
        ASSERT(iterationCount <= 32, "Infinite loop detected! "
            "Maximum iteration in entity processing loop has been reached.");

        // Extract and process command list. This is done to operate only on currently queued
        // commands and not ones that could be subsequently queued in the process.
        CommandList commands;
        commands.swap(m_commands);

        while(!commands.empty())
        {
            // Pop command from queue.
            EntityCommand command = commands.front();
            commands.pop();

            // Retrieve entity entry. Handle may be invalid and command could be out of date.
            auto lookupHandleResult = m_entities.LookupHandle(command.handle);
            if(!lookupHandleResult)
                continue;

            EntityList::HandleEntryRef handleEntry = lookupHandleResult.Unwrap();
            EntityEntry* entityEntry = handleEntry.GetStorage();
            ASSERT(entityEntry != nullptr);

            // Process entity command.
            switch(command.type)
            {
            case EntityCommands::Create:
                {
                    // Inform that new entity was created since last time commands were processed.
                    // This will allow systems to acknowledge this entity and initialize its
                    // components.
                    if(!events.entityCreate(handleEntry.GetHandle()))
                    {
                        // Some system failed to initialize this entity. Destroy the entity
                        // immediately and also inform systems that may have already processed it.
                        events.entityDestroy(handleEntry.GetHandle());
                        m_entities.DestroyHandle(handleEntry.GetHandle());
                        break;
                    }

                    // Mark entity as officially created.
                    ASSERT(entityEntry->flags & EntityFlags::Exists);
                    entityEntry->flags |= EntityFlags::Created;
                }
                break;

            case EntityCommands::Destroy:
                {
                    // Inform about entity being destroyed since last time commands were processed.
                    events.entityDestroy(handleEntry.GetHandle());

                    // Free the entity handle and return it to the pool.
                    ASSERT(entityEntry->flags & EntityFlags::Destroy);
                    m_entities.DestroyHandle(handleEntry.GetHandle());
                }
                break;
            }
        }

        // Increment iteration count for infinite loop prevention.
        ++iterationCount;
    }
}

bool EntitySystem::IsEntityValid(const EntityHandle entity) const
{
    return m_entities.LookupHandle(entity).IsSuccess();
}

bool Game::EntitySystem::IsEntityCreated(const EntityHandle entity) const
{
    if(auto lookupHandleResult = m_entities.LookupHandle(entity))
    {
        EntityList::ConstHandleEntryRef handleEntry = lookupHandleResult.Unwrap();
        const EntityEntry* entityEntry = handleEntry.GetStorage();
        ASSERT(entityEntry != nullptr && entityEntry->flags & EntityFlags::Exists);

        if(!(entityEntry->flags & EntityFlags::Created))
            return false;
        
        return true;
    }
    else
    {
        return false;
    }
}
