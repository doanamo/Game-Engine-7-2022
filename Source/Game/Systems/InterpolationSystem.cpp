/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#include "../Precompiled.hpp"
#include "Game/Systems/InterpolationSystem.hpp"
#include "Game/Components/TransformComponent.hpp"
#include "Game/Components/SpriteAnimationComponent.hpp"
#include "Game/ComponentSystem.hpp"
using namespace Game;

InterpolationSystem::InterpolationSystem() = default;
InterpolationSystem::~InterpolationSystem() = default;

InterpolationSystem::CreateResult InterpolationSystem::Create(ComponentSystem* componentSystem)
{
    LOG("Creating interpolation system...");
    LOG_SCOPED_INDENT();

    // Check arguments.
    CHECK_ARGUMENT_OR_RETURN(componentSystem != nullptr, Common::Failure(CreateErrors::InvalidArgument));

    // Create instance.
    auto instance = std::unique_ptr<InterpolationSystem>(new InterpolationSystem());

    // Save component system reference.
    instance->m_componentSystem = componentSystem;

    // Success!
    return Common::Success(std::move(instance));
}

void InterpolationSystem::Update(float timeDelta)
{
    // Reset interpolation state of all sprite transform components.
    for(auto& transformComponent : m_componentSystem->GetPool<Game::TransformComponent>())
    {
        transformComponent.ResetInterpolation();
    }

    // Reset interpolation states of all sprite animation components.
    for(auto& spriteAnimationComponent : m_componentSystem->GetPool<Game::SpriteAnimationComponent>())
    {
        spriteAnimationComponent.ResetInterpolation();
    }
}
