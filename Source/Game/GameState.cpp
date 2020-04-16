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

GameState::InitializeResult GameState::Initialize()
{
    LOG("Initializing game state...");
    LOG_SCOPED_INDENT();

    // Setup initialization guard.
    VERIFY(!m_initialized, "Instance has already been initialized!");
    SCOPE_GUARD_IF(!m_initialized, this->Reset());

    // Initialize entity system.
    // Assigns unique identifiers that all other systems use to identify objects in a game.
    if(!entitySystem.Initialize())
    {
        LOG_ERROR("Could not initialize entity system!");
        return Failure(InitializeErrors::FailedSubsystemInitialization);
    }

    // Initialize component system.
    // Stores and manages components that entities have.
    if(!componentSystem.Initialize(&entitySystem))
    {
        LOG_ERROR("Could not initialize component system!");
        return Failure(InitializeErrors::FailedSubsystemInitialization);
    }

    // Initialize identity system.
    // Allows readable names to be assigned to entities.
    if(!identitySystem.Initialize(&entitySystem))
    {
        LOG_ERROR("Could not initialize identity system!");
        return Failure(InitializeErrors::FailedSubsystemInitialization);
    }

    // Initialize interpolation system.
    // Controls how and when entities are interpolated.
    if(!interpolationSystem.Initialize(&componentSystem))
    {
        LOG_ERROR("Could not initialize interpolation system!");
        return Failure(InitializeErrors::FailedSubsystemInitialization);
    }

    // Initialize sprite system.
    // Updates sprites and their animations.
    if(!spriteSystem.Initialize(&componentSystem))
    {
        LOG_ERROR("Could not initialize sprite system!");
        return Failure(InitializeErrors::FailedSubsystemInitialization);
    }

    // Initialize update timer.
    if(!updateTimer.Initialize())
    {
        LOG_ERROR("Could not initialize update timer!");
        return Failure(InitializeErrors::FailedSubsystemInitialization);
    }

    // Bind and subscribe event receivers.
    m_changeUpdateTime.Bind([this](const Events::ChangeUpdateTime& event) -> bool
    {
        m_updateTime = event.updateTime;
        return true;
    });

    eventBroker.Subscribe(m_changeUpdateTime);

    // Success!
    m_initialized = true;
    return Success();
}

void GameState::PushEvent(std::any event)
{
    ASSERT(m_initialized, "Game state has not been initialized!");

    // Add event to be processed later.
    eventQueue.Push(event);
}

bool GameState::Update(const System::Timer& timer)
{
    ASSERT(m_initialized, "Game state has not been initialized!");

    // Inform about update being called.
    events.updateCalled.Dispatch();

    // Tick update timer along with the application timer.
    updateTimer.Tick(timer);

    // Return flag indicating if state was updated.
    bool stateUpdated = false;

    // Main game state update loop.
    const float updateTime = m_updateTime;

    while(updateTimer.Update(updateTime))
    {
        // Process events.
        while(!eventQueue.IsEmpty())
        {
            std::any event = eventQueue.Pop();
            eventBroker.Dispatch(event);
        }

        // Process entity commands.
        entitySystem.ProcessCommands();

        // Update interpolation system.
        interpolationSystem.Update(updateTime);

        // Update sprite animation system.
        spriteSystem.Update(updateTime);

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
    ASSERT(m_initialized, "Game state has not been initialized!");
    return m_updateTime;
}
