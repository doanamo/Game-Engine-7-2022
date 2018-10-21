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

    // Get all sprite animation components.
    for(auto it = m_componentSystem->Begin<SpriteAnimationComponent>();
        it != m_componentSystem->End<SpriteAnimationComponent>(); ++it)
    {
        // Get the sprite animation component.
        // #todo: Create a custom ComponentIterator to access elements in ComponentPool.
        // We can modify a component handle and cause undefined behavior if we want.
        SpriteAnimationComponent& spriteAnimationComponent = it->component;
        
        // Get the sprite component.
        SpriteComponent* spriteComponent = spriteAnimationComponent.GetSpriteComponent();

        // Update the sprite animation component.
        spriteAnimationComponent.Update(timeDelta);

        // Skip sprite frame change if animation is not playing.
        if(!spriteAnimationComponent.IsPlaying())
            continue;

        // Find the current frame.
        // #todo: We are iterating through the entire list of frames here.
        // Performance here is slower the more frames are in an animation.
        // We should be just using a looping index or a ring iterator.
        auto spriteAnimation = spriteAnimationComponent.GetSpriteAnimation();
        float playbackTime = spriteAnimationComponent.GetPlaybackTime();

        for(const auto& frame : spriteAnimation->frames)
        {
            // Check if the time is within the current frame.
            if(playbackTime <= frame.duration)
            {
                // Set texture view from the current frame.
                spriteComponent->SetTextureView(frame.textureView);
                break;
            }

            // Proceed to the next frame.
            playbackTime -= frame.duration;
        }
    }
}
