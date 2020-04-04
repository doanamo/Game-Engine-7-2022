/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#include "Game/Systems/InterpolationSystem.hpp"
#include "Game/Components/TransformComponent.hpp"
#include "Game/Components/SpriteAnimationComponent.hpp"
#include "Game/ComponentSystem.hpp"
using namespace Game;

InterpolationSystem::InterpolationSystem() :
    m_componentSystem(nullptr),
    m_initialized(false)
{
}

InterpolationSystem::~InterpolationSystem()
{
}

InterpolationSystem::InterpolationSystem(InterpolationSystem&& other) :
    InterpolationSystem()
{
    *this = std::move(other);
}

InterpolationSystem& InterpolationSystem::operator=(InterpolationSystem&& other)
{
    std::swap(m_componentSystem, other.m_componentSystem);
    std::swap(m_initialized, other.m_initialized);

    return *this;
}

bool InterpolationSystem::Initialize(ComponentSystem* componentSystem)
{
    LOG("Initializing interpolation system...");
    LOG_SCOPED_INDENT();

    // Make sure this instance has not been initialized yet.
    ASSERT(!m_initialized, "Interpolation systems has already been initialized!");

    // Create a scope guard.
    SCOPE_GUARD_IF(!m_initialized, *this = InterpolationSystem());

    // Validate component system reference.
    if(componentSystem == nullptr)
    {
        LOG_ERROR("Invalid argument - \"componentSystem\" is null!");
        return false;
    }

    m_componentSystem = componentSystem;

    // Success!
    return m_initialized = true;
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
