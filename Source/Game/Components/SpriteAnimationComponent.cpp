/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#include "Game/Components/SpriteAnimationComponent.hpp"
#include "Game/Components/SpriteComponent.hpp"
#include "Game/ComponentSystem.hpp"
#include <Graphics/Sprite/SpriteAnimationList.hpp>
using namespace Game;

SpriteAnimationComponent::SpriteAnimationComponent() = default;
SpriteAnimationComponent::~SpriteAnimationComponent() = default;

bool SpriteAnimationComponent::OnInitialize(ComponentSystem* componentSystem, const EntityHandle& entitySelf)
{
    m_spriteComponent = componentSystem->Lookup<SpriteComponent>(entitySelf);
    if(m_spriteComponent == nullptr)
        return false;

    return true;
}

void SpriteAnimationComponent::SetSpriteAnimationList(SpriteAnimationListPtr spriteAnimationList)
{
    // Stop currently playing animation.
    this->Stop();

    // Set new sprite animation resource.
    m_spriteAnimationList = spriteAnimationList;
}

void SpriteAnimationComponent::ResetInterpolation()
{
    // Wrap or clamp the current animation time.
    if(m_spriteAnimation != nullptr)
    {
        if(m_playbackInfo & PlaybackFlags::Loop)
        {
            m_currentAnimationTime = glm::mod(m_currentAnimationTime, m_spriteAnimation->duration);
        }
        else
        {
            m_currentAnimationTime = glm::min(m_currentAnimationTime, m_spriteAnimation->duration);
        }
    }

    // Update animation time interpolation.
    m_previousAnimationTime = m_currentAnimationTime;
}

void SpriteAnimationComponent::Play(std::string animationName, bool loop)
{
    // Make sure that we have a sprite animation list set.
    if(m_spriteAnimationList == nullptr)
    {
        LOG_WARNING("Attempting to play \"{}\" sprite animation without sprite animation list set!", animationName);
        return;
    }

    // Get animation index.
    std::optional<std::size_t> animationIndex = m_spriteAnimationList->GetAnimationIndex(animationName);

    if(!animationIndex.has_value())
    {
        LOG_WARNING("Could not find \"{}\" sprite animation to play!", animationName);
        return;
    }

    // Start playing an animation.
    m_spriteAnimation = m_spriteAnimationList->GetAnimationByIndex(animationIndex.value());
    ASSERT(m_spriteAnimation, "Sprite animation retrieved via index is null!");

    m_playbackInfo = PlaybackFlags::Playing;
    m_currentAnimationTime = 0.0f;
    m_previousAnimationTime = 0.0f;
    
    // Set playback loop flag.
    if(loop)
    {
        m_playbackInfo |= PlaybackFlags::Loop;
    }
}

bool SpriteAnimationComponent::IsPlaying() const
{
    return m_playbackInfo & PlaybackFlags::Playing;
}

void SpriteAnimationComponent::Pause()
{
    // Pause currently playing animation.
    m_playbackInfo &= ~PlaybackFlags::Playing;
}

void SpriteAnimationComponent::Resume()
{
    // Resume currently playing animation.
    if(m_spriteAnimation != nullptr)
    {
        m_playbackInfo |= PlaybackFlags::Playing;
    }
}

void SpriteAnimationComponent::Stop()
{
    // Stop currently playing animation.
    m_spriteAnimation = nullptr;
    m_playbackInfo = PlaybackFlags::None;

    m_currentAnimationTime = 0.0f;
    m_previousAnimationTime = 0.0f;
}

void SpriteAnimationComponent::Update(float timeDelta)
{
    // Increment playback time.
    if(m_playbackInfo & PlaybackFlags::Playing)
    {
        ASSERT(m_spriteAnimation, "Playing sprite animation without an animation set!");
        ASSERT(m_spriteAnimation->duration >= 0.0f, "Sprite animation has an invalid duration!");

        // Update the current animation time.
        m_currentAnimationTime += timeDelta;

        // Check if animation ended in case we do not want it to loop.
        if(!(m_playbackInfo & PlaybackFlags::Loop))
        {
            if(m_currentAnimationTime >= m_spriteAnimation->duration)
            {
                m_playbackInfo &= ~PlaybackFlags::Playing;
            }
        }
    }
}

float SpriteAnimationComponent::CalculateAnimationTime(float timeAlpha) const
{
    ASSERT(timeAlpha >= 0.0f && timeAlpha <= 1.0f, "Time alpha is not normalized!");
    ASSERT(m_spriteAnimation, "Cannot calculate animation time without sprite animation set!");

    // Return wrapped or clamped animation time (when duration is shorter).
    float animationTime = glm::mix(m_previousAnimationTime, m_currentAnimationTime, timeAlpha);

    if(m_playbackInfo & PlaybackFlags::Loop)
    {
        return glm::mod(animationTime, m_spriteAnimation->duration);
    }
    else
    {
        return glm::min(animationTime, m_spriteAnimation->duration);
    }
}

const SpriteAnimationComponent::SpriteAnimationListPtr& SpriteAnimationComponent::GetSpriteAnimationList() const
{
    return m_spriteAnimationList;
}

const SpriteAnimationComponent::SpriteAnimation* SpriteAnimationComponent::GetSpriteAnimation() const
{
    return m_spriteAnimation;
}

SpriteComponent* SpriteAnimationComponent::GetSpriteComponent() const
{
    return m_spriteComponent;
}
