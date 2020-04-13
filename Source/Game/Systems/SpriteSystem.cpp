/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#include "Game/Systems/SpriteSystem.hpp"
#include "Game/Components/SpriteAnimationComponent.hpp"
#include "Game/Components/SpriteComponent.hpp"
#include "Game/ComponentSystem.hpp"
#include <Graphics/Sprite/SpriteAnimationList.hpp>
using namespace Game;

SpriteSystem::SpriteSystem(SpriteSystem&& other) :
    SpriteSystem()
{
    *this = std::move(other);
}

SpriteSystem& SpriteSystem::operator=(SpriteSystem&& other)
{
    std::swap(m_componentSystem, other.m_componentSystem);
    std::swap(m_initialized, other.m_initialized);

    return *this;
}

bool SpriteSystem::Initialize(ComponentSystem* componentSystem)
{
    LOG("Initializing sprite system...");
    LOG_SCOPED_INDENT();

    // Make sure that the instance has not been initialized yet.
    ASSERT(!m_initialized, "Sprite system has already been initialized!");

    // Create initialization scope guard.
    SCOPE_GUARD_IF(!m_initialized, *this = SpriteSystem());

    // Validate component system reference.
    if(componentSystem == nullptr)
    {
        LOG_ERROR("Invalid arguments - \"componentSystem\" is null!");
        return false;
    }

    m_componentSystem = componentSystem;

    // Success!
    return m_initialized = true;
}

void SpriteSystem::Update(float timeDelta)
{
    ASSERT(m_initialized, "Sprite system has not been initialized!");

    // Get all sprite animation components.
    for(auto& spriteAnimationComponent : m_componentSystem->GetPool<SpriteAnimationComponent>())
    {
        // Update the sprite animation component.
        spriteAnimationComponent.Update(timeDelta);
    }
}
