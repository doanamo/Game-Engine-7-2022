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
    events.instanceDestroyed.Dispatch();
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
    // Controls sprites and their animations.
    instance->spriteSystem = SpriteSystem::Create(instance->componentSystem.get()).UnwrapOr(nullptr);
    if(instance->spriteSystem == nullptr)
    {
        LOG_ERROR("Could not create sprite system!");
        return Common::Failure(CreateErrors::FailedSubsystemCreation);
    }

    // Create tick timer.
    instance->tickTimer = TickTimer::Create().UnwrapOr(nullptr);
    if(instance->tickTimer == nullptr)
    {
        LOG_ERROR("Could not create tick timer!");
        return Common::Failure(CreateErrors::FailedSubsystemCreation);
    }

    // Success!
    return Common::Success(std::move(instance));
}

bool GameState::Tick(const System::Timer& timer)
{
    // Return flag indicating if state was ticked.
    bool stateTicked = false;

    // Inform about tick method being called.
    events.tickRequested.Dispatch();

    // Advance tick timer.
    tickTimer->Advance(timer);

    // Main game state tick loop.
    while(tickTimer->Tick())
    {
        // Retrieve last tick time.
        float tickTime = tickTimer->GetLastTickSeconds();

        // Process entity commands.
        entitySystem->ProcessCommands();

        // Tick interpolation system.
        interpolationSystem->Tick(tickTime);

        // Tick sprite animation system.
        spriteSystem->Tick(tickTime);

        // Inform that state had its tick processed.
        // Allows for custom tick logic to be executed.
        events.tickProcessed.Dispatch(tickTime);

        // State has been ticked at least once.
        stateTicked = true;
    }

    // Return whether state could be ticked.
    return stateTicked;
}
