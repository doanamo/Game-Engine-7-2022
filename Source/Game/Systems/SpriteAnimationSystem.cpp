/*
    Copyright (c) 2018 Piotr Doan. All rights reserved.
*/

#include "Precompiled.hpp"
#include "Game/Systems/SpriteAnimationSystem.hpp"
#include "Game/Components/SpriteAnimationComponent.hpp"
#include "Game/Components/SpriteComponent.hpp"
#include "Game/ComponentSystem.hpp"
#include "Graphics/Sprites/SpriteAnimationList.hpp"
using namespace Game;

SpriteAnimationSystem::SpriteAnimationSystem() :
    m_componentSystem(nullptr),
    m_initialized(false)
{
}

SpriteAnimationSystem::~SpriteAnimationSystem()
{
}

SpriteAnimationSystem::SpriteAnimationSystem(SpriteAnimationSystem&& other)
{
    *this = std::move(other);
}

SpriteAnimationSystem& SpriteAnimationSystem::operator=(SpriteAnimationSystem&& other)
{
    std::swap(m_componentSystem, other.m_componentSystem);
    std::swap(m_initialized, other.m_initialized);

    return *this;
}

bool SpriteAnimationSystem::Initialize(ComponentSystem* componentSystem)
{
    LOG() << "Initializing sprite animation system..." << LOG_INDENT();

    // Make sure that the instance has not been initialized yet.
    ASSERT(!m_initialized, "Sprite animation system has already been initialized!");

    // Create an initialization scope guard.
    SCOPE_GUARD_IF(!m_initialized, *this = SpriteAnimationSystem());

    // Validate component system reference.
    if(componentSystem == nullptr)
    {
        LOG_ERROR() << "Invalid arguments - \"componentSystem\" is null!";
        return false;
    }

    m_componentSystem = componentSystem;

    // Success!
    return m_initialized = true;
}

void SpriteAnimationSystem::Update(float timeDelta)
{
    ASSERT(m_initialized, "Sprite animation system has not been initialized!");

    // Get the sprite animation component pool.
    ComponentPool<SpriteAnimationComponent>* spriteAnimationComponentPool =
        m_componentSystem->GetPool<SpriteAnimationComponent>();

    if(spriteAnimationComponentPool == nullptr)
        return;

    // Get all sprite animation components.
    for(auto& spriteAnimationComponent : *spriteAnimationComponentPool)
    {
        // Update the sprite animation component.
        spriteAnimationComponent.Update(timeDelta);

        // Update the sprite texture view.
        if(spriteAnimationComponent.HasFrameChanged())
        {
            SpriteComponent* spriteComponent = spriteAnimationComponent.GetSpriteComponent();

            auto spriteAnimation = spriteAnimationComponent.GetSpriteAnimation();
            std::size_t frameIndex = spriteAnimationComponent.GetFrameIndex();

            spriteComponent->SetTextureView(spriteAnimation->frames[frameIndex].textureView);
        }
    }
}
