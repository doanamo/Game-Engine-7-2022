/*
    Copyright (c) 2018 Piotr Doan. All rights reserved.
*/

#include "Precompiled.hpp"
#include "Game/Components/SpriteAnimationComponent.hpp"
#include "Game/Components/SpriteComponent.hpp"
#include "Game/ComponentSystem.hpp"
#include "Graphics/Sprites/SpriteAnimationList.hpp"
using namespace Game;

SpriteAnimationComponent::SpriteAnimationComponent() :
    m_spriteComponent(nullptr),
    m_spriteAnimationList(),
    m_spriteAnimation(nullptr),
    m_playbackTime(0.0f),
    m_playing(false),
    m_loop(true)
{
}

SpriteAnimationComponent::SpriteAnimationComponent(SpriteAnimationComponent&& other)
{
    *this = std::move(other);
}

SpriteAnimationComponent& SpriteAnimationComponent::operator=(SpriteAnimationComponent&& other)
{
    std::swap(m_spriteComponent, other.m_spriteComponent);
    std::swap(m_spriteAnimationList, other.m_spriteAnimationList);
    std::swap(m_spriteAnimation, other.m_spriteAnimation);
    std::swap(m_playbackTime, other.m_playbackTime);
    std::swap(m_playing, other.m_playing);
    std::swap(m_loop, other.m_loop);

    return *this;
}

bool SpriteAnimationComponent::OnInitialize(ComponentSystem* componentSystem, const EntityHandle& entitySelf)
{
    m_spriteComponent = componentSystem->Lookup<SpriteComponent>(entitySelf);
    if(m_spriteComponent == nullptr) return false;

    return true;
}

void SpriteAnimationComponent::Update(float timeDelta)
{
    // Increment playback time.
    if(m_playing)
    {
        // We should have a sprite animation set.
        ASSERT(m_spriteAnimation, "Playing sprite animation without an animation set!");

        // Update the playback timer.
        m_playbackTime += timeDelta;

        // Handle looping or animation end.
        ASSERT(m_spriteAnimation->duration > 0.0f, "Sprite animation has an invalid duration!");

        if(m_playbackTime >= m_spriteAnimation->duration)
        {
            if(m_loop)
            {
                m_playbackTime = std::fmod(m_playbackTime, m_spriteAnimation->duration);
            }
            else
            {
                m_playbackTime = m_spriteAnimation->duration;
                m_playing = false;
            }
        }
    }
}

void SpriteAnimationComponent::Play(std::string animationName, bool loop)
{
    // Make sure that we have a sprite animation list set.
    if(m_spriteAnimationList == nullptr)
    {
        LOG_WARNING() << "Attempting to play a sprite animation without sprite animation list set!";
        return;
    }

    // Get animation sequence index.
    std::optional<std::size_t> sequenceIndex = m_spriteAnimationList->GetAnimationIndex(animationName);

    if(!sequenceIndex.has_value())
    {
        LOG_WARNING() << "Could not find \"" << animationName << "\" sprite animation to play!";
        return;
    }

    // Start playing an animation.
    m_spriteAnimation = m_spriteAnimationList->GetAnimationByIndex(sequenceIndex.value());
    m_playbackTime = 0.0f;
    m_playing = true;
    m_loop = loop;
}

void SpriteAnimationComponent::SetSpriteAnimationList(SpriteAnimationListPtr spriteAnimationList)
{
    // Set new sprite animation resource.
    m_spriteAnimationList = spriteAnimationList;

    // Stop and reset all playback state.
    m_spriteAnimation = nullptr;
    m_playbackTime = 0.0f;
    m_playing = false;
    m_loop = false;
}

SpriteComponent* SpriteAnimationComponent::GetSpriteComponent() const
{
    return m_spriteComponent;
}

const SpriteAnimationComponent::SpriteAnimationListPtr& SpriteAnimationComponent::GetSpriteAnimationList() const
{
    return m_spriteAnimationList;
}

const SpriteAnimationComponent::SpriteAnimation* SpriteAnimationComponent::GetSpriteAnimation() const
{
    return m_spriteAnimation;
}

float SpriteAnimationComponent::GetPlaybackTime() const
{
    return m_playbackTime;
}

bool SpriteAnimationComponent::IsPlaying() const
{
    return m_playing;
}
