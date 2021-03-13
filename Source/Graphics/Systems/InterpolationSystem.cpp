/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#include "Graphics/Precompiled.hpp"
#include "Graphics/Systems/InterpolationSystem.hpp"
#include "Graphics/Components/SpriteAnimationComponent.hpp"
#include <Game/Components/TransformComponent.hpp>
#include <Game/ComponentSystem.hpp>
using namespace Graphics;

InterpolationSystem::InterpolationSystem() = default;
InterpolationSystem::~InterpolationSystem() = default;

InterpolationSystem::CreateResult InterpolationSystem::Create(Game::ComponentSystem* componentSystem)
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

void InterpolationSystem::Tick(float timeDelta)
{
    // Reset interpolation state of all sprite transform components.
    for(auto& transformComponent : m_componentSystem->GetPool<Game::TransformComponent>())
    {
        transformComponent.ResetInterpolation();
    }

    // Reset interpolation states of all sprite animation components.
    for(auto& spriteAnimationComponent : m_componentSystem->GetPool<SpriteAnimationComponent>())
    {
        spriteAnimationComponent.ResetInterpolation();
    }
}
