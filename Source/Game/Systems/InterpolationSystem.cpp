/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#include "Game/Systems/InterpolationSystem.hpp"
#include "Game/Components/TransformComponent.hpp"
#include "Game/Components/SpriteAnimationComponent.hpp"
#include "Game/ComponentSystem.hpp"
using namespace Game;

InterpolationSystem::InterpolationSystem() = default;
InterpolationSystem::~InterpolationSystem() = default;

InterpolationSystem::InitializeResult InterpolationSystem::Initialize(ComponentSystem* componentSystem)
{
    LOG("Initializing interpolation system...");
    LOG_SCOPED_INDENT();

    // Setup initialization guard.
    VERIFY(!m_initialized, "Instance has already been initialized!");
    SCOPE_GUARD_IF(!m_initialized, this->Reset());

    // Validate component system reference.
    CHECK_ARGUMENT_OR_RETURN(componentSystem != nullptr, Failure(InitializeErrors::InvalidArgument));

    m_componentSystem = componentSystem;

    // Success!
    m_initialized = true;
    return Success();
}

void InterpolationSystem::Update(float timeDelta)
{
    ASSERT(m_initialized, "Interpolation system is not initialized!");

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
