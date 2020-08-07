/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#include "Precompiled.hpp"
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
        return Common::Failure(CreateErrors::FailedSubsystemCreation);
    }

    // Create component system.
    // Stores and manages components that entities have.
    instance->componentSystem = ComponentSystem::Create(instance->entitySystem.get()).UnwrapOr(nullptr);
    if(instance->componentSystem == nullptr)
    {
        LOG_ERROR("Could not create component system!");
        return Common::Failure(CreateErrors::FailedSubsystemCreation);
    }

    // Create identity system.
    // Allows readable names to be assigned to entities.
    instance->identitySystem = IdentitySystem::Create(instance->entitySystem.get()).UnwrapOr(nullptr);
    if(instance->identitySystem == nullptr)
    {
        LOG_ERROR("Could not create identity system!");
        return Common::Failure(CreateErrors::FailedSubsystemCreation);
    }

    // Create interpolation system.
    // Controls how and when entities are interpolated.
    instance->interpolationSystem = InterpolationSystem::Create(instance->componentSystem.get()).UnwrapOr(nullptr);
    if(instance->interpolationSystem == nullptr)
    {
        LOG_ERROR("Could not create interpolation system!");
        return Common::Failure(CreateErrors::FailedSubsystemCreation);
    }

    // Create sprite system.
    // Updates sprites and their animations.
    instance->spriteSystem = SpriteSystem::Create(instance->componentSystem.get()).UnwrapOr(nullptr);
    if(instance->spriteSystem == nullptr)
    {
        LOG_ERROR("Could not create sprite system!");
        return Common::Failure(CreateErrors::FailedSubsystemCreation);
    }

    // Create update timer.
    instance->updateTimer = UpdateTimer::Create().UnwrapOr(nullptr);
    if(instance->updateTimer == nullptr)
    {
        LOG_ERROR("Could not create update timer!");
        return Common::Failure(CreateErrors::FailedSubsystemCreation);
    }

    // Success!
    return Common::Success(std::move(instance));
}

void GameState::ChangeUpdateTime(float updateTime)
{
    m_updateTime = updateTime;
}

bool GameState::Update(const System::Timer& timer)
{
    // Inform about update being called.
    events.updateCalled.Dispatch();

    // Advance update timer.
    updateTimer->Advance(timer);

    // Return flag indicating if state was updated.
    bool stateUpdated = false;

    // Main game state update loop.
    // Make copy of update time in case it changes.
    const float updateTime = m_updateTime;

    while(updateTimer->Update(updateTime))
    {
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
