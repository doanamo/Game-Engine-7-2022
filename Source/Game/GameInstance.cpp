/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#include "Precompiled.hpp"
#include "Game/GameInstance.hpp"
using namespace Game;

GameInstance::GameInstance() = default;
GameInstance::~GameInstance() = default;

GameInstance::CreateResult GameInstance::Create()
{
    LOG("Creating game instance...");
    LOG_SCOPED_INDENT();

    // Create instance.
    auto instance = std::unique_ptr<GameInstance>(new GameInstance());

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

    // Success!
    return Common::Success(std::move(instance));
}

void GameInstance::PreTick(const float tickTime)
{
    // Process entity commands.
    entitySystem->ProcessCommands();

    // Tick interpolation system.
    interpolationSystem->Tick(tickTime);

    // Tick sprite animation system.
    spriteSystem->Tick(tickTime);
}

void GameInstance::PostTick(const float tickTime)
{
}
