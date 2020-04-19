/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#include "Game/GameState.hpp"
using namespace Game;

GameState::GameState() = default;

GameState::~GameState()
{
    // Notify about game state instance being destructed.
    events.instanceDestructed.Dispatch();
}

GameState::CreateResult GameState::Create()
{
    LOG("Creating game state...");
    LOG_SCOPED_INDENT();

    // Create instance.
    auto instance = std::unique_ptr<GameState>(new GameState());

    // Create entity system.
    // Assigns unique identifiers that all other systems use to identify objects in a game.
    instance->entitySystem = EntitySystem::Create().UnwrapOr(nullptr);
    if(instance->entitySystem == nullptr)
    {
        LOG_ERROR("Could not create entity system!");
        return Failure(CreateErrors::FailedSubsystemInitialization);
    }

    // Create component system.
    // Stores and manages components that entities have.
    instance->componentSystem = ComponentSystem::Create(instance->entitySystem.get()).UnwrapOr(nullptr);
    if(instance->componentSystem == nullptr)
    {
        LOG_ERROR("Could not create component system!");
        return Failure(CreateErrors::FailedSubsystemInitialization);
    }

    // Create identity system.
    // Allows readable names to be assigned to entities.
    instance->identitySystem = IdentitySystem::Create(instance->entitySystem.get()).UnwrapOr(nullptr);
    if(instance->identitySystem == nullptr)
    {
        LOG_ERROR("Could not create identity system!");
        return Failure(CreateErrors::FailedSubsystemInitialization);
    }

    // Create interpolation system.
    // Controls how and when entities are interpolated.
    instance->interpolationSystem = InterpolationSystem::Create(instance->componentSystem.get()).UnwrapOr(nullptr);
    if(instance->interpolationSystem == nullptr)
    {
        LOG_ERROR("Could not create interpolation system!");
        return Failure(CreateErrors::FailedSubsystemInitialization);
    }

    // Create sprite system.
    // Updates sprites and their animations.
    instance->spriteSystem = SpriteSystem::Create(instance->componentSystem.get()).UnwrapOr(nullptr);
    if(instance->spriteSystem == nullptr)
    {
        LOG_ERROR("Could not create sprite system!");
        return Failure(CreateErrors::FailedSubsystemInitialization);
    }

    // Initialize update timer.
    instance->updateTimer = UpdateTimer::Create().UnwrapOr(nullptr);
    if(instance->updateTimer == nullptr)
    {
        LOG_ERROR("Could not create update timer!");
        return Failure(CreateErrors::FailedSubsystemInitialization);
    }

    // Bind and subscribe event receivers.
    GameState* instancePtr = instance.get();
    instance->m_changeUpdateTime.Bind([instancePtr](const Events::ChangeUpdateTime& event) -> bool
    {
        instancePtr->m_updateTime = event.updateTime;
        return true;
    });

    instance->eventBroker.Subscribe(instance->m_changeUpdateTime);

    // Success!
    return Success(std::move(instance));
}

void GameState::PushEvent(std::any event)
{
    // Add event to be processed later.
    eventQueue.Push(event);
}

bool GameState::Update(const System::Timer& timer)
{
    // Inform about update being called.
    events.updateCalled.Dispatch();

    // Tick update timer along with the application timer.
    updateTimer->Tick(timer);

    // Return flag indicating if state was updated.
    bool stateUpdated = false;

    // Main game state update loop.
    const float updateTime = m_updateTime;

    while(updateTimer->Update(updateTime))
    {
        // Process events.
        while(!eventQueue.IsEmpty())
        {
            std::any event = eventQueue.Pop();
            eventBroker.Dispatch(event);
        }

        // Process entity commands.
        entitySystem->ProcessCommands();

        // Update interpolation system.
        interpolationSystem->Update(updateTime);

        // Update sprite animation system.
        spriteSystem->Update(updateTime);

        // Inform that state had its update processed.
        // Allows for custom update logic to be executed.
        events.updateProcessed.Dispatch(updateTime);

        // State has been updated at least once.
        stateUpdated = true;
    }

    // Return whether state could be updated.
    return stateUpdated;
}

float GameState::GetUpdateTime() const
{
    return m_updateTime;
}
