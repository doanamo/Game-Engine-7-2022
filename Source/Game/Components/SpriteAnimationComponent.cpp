/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#include "Game/Precompiled.hpp"
#include "Game/Components/SpriteAnimationComponent.hpp"
#include "Game/Components/SpriteComponent.hpp"
#include "Game/ComponentSystem.hpp"
#include <Graphics/Sprite/SpriteAnimationList.hpp>
using namespace Game;

SpriteAnimationComponent::SpriteAnimationComponent() = default;
SpriteAnimationComponent::~SpriteAnimationComponent() = default;

bool SpriteAnimationComponent::OnInitialize(ComponentSystem* componentSystem,
    const EntityHandle& entitySelf)
{
    // Retrieve needed components.
    m_spriteComponent = componentSystem->Lookup<SpriteComponent>(entitySelf).UnwrapOr(nullptr);
    if(m_spriteComponent == nullptr)
        return false;

    return true;
}

void SpriteAnimationComponent::SetSpriteAnimationList(SpriteAnimationListPtr spriteAnimationList)
{
    if(IsPlaying())
    {
        Stop();
    }

    m_spriteAnimationList = spriteAnimationList;
}

void SpriteAnimationComponent::ResetInterpolation()
{
    /*
        Proceed to end of next interpolation range with assurance that overflown animation time is
        handled without accumulating time error when animation is looping.
    */
    
    if(m_currentSpriteAnimation)
    {
        m_currentAnimationTime = CalculateAnimationTime(1.0f);
        m_previousAnimationTime = m_currentAnimationTime;
    }
}

void SpriteAnimationComponent::Tick(float timeDelta)
{
    /*
        Update playback time and check if animation is still in playing state.
    */

    if(IsPlaying())
    {
        ASSERT(m_currentSpriteAnimation, "Current sprite animation without an animation set!");
        ASSERT(m_currentSpriteAnimation->duration >= 0.0f, "Sprite animation has an invalid duration!");

        m_currentAnimationTime += timeDelta;

        if(!IsLooped())
        {
            if(m_currentAnimationTime >= m_currentSpriteAnimation->duration)
            {
                Pause();
            }
        }
    }
}

void SpriteAnimationComponent::Play(std::string animationName, bool loop)
{
    if(!m_spriteAnimationList)
    {
        LOG_ERROR("Cannot play \"{}\" sprite animation"
            "without sprite animation list set!", animationName);
        return;
    }

    if(auto animationIndexResult = m_spriteAnimationList->GetAnimationIndex(animationName))
    {
        uint32_t animationIndex = animationIndexResult.Unwrap();
        m_currentSpriteAnimation = m_spriteAnimationList->GetAnimationByIndex(animationIndex);
        ASSERT(m_currentSpriteAnimation, "Sprite animation retrieved via its index is null!");

        m_playbackInfo = PlaybackFlags::Playing;
        m_currentAnimationTime = 0.0f;
        m_previousAnimationTime = 0.0f;

        if(loop)
        {
            m_playbackInfo |= PlaybackFlags::Loop;
        }
    }
    else
    {
        LOG_ERROR("Could not find \"{}\" sprite animation to play!", animationName);
    }
}

void SpriteAnimationComponent::Pause()
{
    m_playbackInfo &= ~PlaybackFlags::Playing;
}

void SpriteAnimationComponent::Resume()
{
    if(m_currentSpriteAnimation)
    {
        m_playbackInfo |= PlaybackFlags::Playing;
    }
}

void SpriteAnimationComponent::Stop()
{
    m_currentSpriteAnimation = nullptr;
    m_playbackInfo = PlaybackFlags::None;
    m_currentAnimationTime = 0.0f;
    m_previousAnimationTime = 0.0f;
}

float SpriteAnimationComponent::CalculateAnimationTime(float timeAlpha) const
{
    /*
        Calculate current animation time based on frame time alpha and handle looping time in such
        way that avoid time error from being accumulated.
    */

    ASSERT(timeAlpha >= 0.0f && timeAlpha <= 1.0f, "Time alpha is not normalized!");
    ASSERT(m_currentSpriteAnimation, "Cannot calculate animation time without sprite animation set!");

    float animationTime = glm::mix(m_previousAnimationTime, m_currentAnimationTime, timeAlpha);

    if(IsLooped())
    {
        return glm::mod(animationTime, m_currentSpriteAnimation->duration);
    }
    else
    {
        return glm::min(animationTime, m_currentSpriteAnimation->duration);
    }
}
