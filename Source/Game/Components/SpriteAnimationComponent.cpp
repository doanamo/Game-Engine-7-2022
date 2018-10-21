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
    m_frameTime(0.0f),
    m_frameIndex(0),
    m_playbackInfo(PlaybackFlags::None),
    m_playbackTime(0.0f)
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
    std::swap(m_frameTime, other.m_frameTime);
    std::swap(m_frameIndex, other.m_frameIndex);
    std::swap(m_playbackInfo, other.m_playbackInfo);
    std::swap(m_playbackTime, other.m_playbackTime);

    return *this;
}

bool SpriteAnimationComponent::OnInitialize(ComponentSystem* componentSystem, const EntityHandle& entitySelf)
{
    m_spriteComponent = componentSystem->Lookup<SpriteComponent>(entitySelf);
    if(m_spriteComponent == nullptr) return false;

    return true;
}

void SpriteAnimationComponent::SetSpriteAnimationList(SpriteAnimationListPtr spriteAnimationList)
{
    // Stop currently playing animation.
    this->Stop();

    // Set new sprite animation resource.
    m_spriteAnimationList = spriteAnimationList;
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
    ASSERT(m_spriteAnimation, "Sprite animation retrieved via index is null!");

    m_frameTime = 0.0f;
    m_frameIndex = 0;

    m_playbackInfo = PlaybackFlags::Playing | PlaybackFlags::FirstFrame;
    m_playbackTime = 0.0f;
    
    // Set playback loop flag.
    if(loop)
    {
        m_playbackInfo |= PlaybackFlags::Loop;
    }
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

    m_frameTime = 0.0f;
    m_frameIndex = 0;

    m_playbackInfo = PlaybackFlags::None;
    m_playbackTime = 0.0f;
}

void SpriteAnimationComponent::Update(float timeDelta)
{
    // Increment playback time.
    if(m_playbackInfo & PlaybackFlags::Playing)
    {
        ASSERT(m_spriteAnimation, "Playing sprite animation without an animation set!");
        ASSERT(m_spriteAnimation->duration > 0.0f, "Sprite animation has an invalid duration!");

        // Clear the frame changed flag before every update to avoid unnecessary frame changes.
        // One exception is the first frame after animation starts playing.
        if(m_playbackInfo & PlaybackFlags::FirstFrame)
        {
            m_playbackInfo |= PlaybackFlags::FrameChanged;
            m_playbackInfo &= ~PlaybackFlags::FirstFrame;
        }
        else
        {
            m_playbackInfo &= ~PlaybackFlags::FrameChanged;
        }

        // Update current frame index.
        m_frameTime += timeDelta;

        while(m_frameTime > m_spriteAnimation->frames[m_frameIndex].duration)
        {
            m_frameTime -= m_spriteAnimation->frames[m_frameIndex].duration;
            m_frameIndex = (m_frameIndex + 1) % m_spriteAnimation->frames.size();
            m_playbackInfo |= PlaybackFlags::FrameChanged;
        }

        // Update the playback timer.
        m_playbackTime += timeDelta;

        if(m_playbackTime > m_spriteAnimation->duration)
        {
            if(m_playbackInfo & PlaybackFlags::Loop)
            {
                m_playbackTime = std::fmod(m_playbackTime, m_spriteAnimation->duration);
            }
            else
            {
                m_frameIndex = m_spriteAnimation->frames.size() - 1;
                m_frameTime = m_spriteAnimation->frames[m_frameIndex].duration;

                m_playbackTime = m_spriteAnimation->duration;
                m_playbackInfo &= ~PlaybackFlags::Playing;
            }
        }
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

std::size_t SpriteAnimationComponent::GetFrameIndex() const
{
    return m_frameIndex;
}

bool SpriteAnimationComponent::HasFrameChanged() const
{
    return m_playbackInfo & PlaybackFlags::FrameChanged;
}

float SpriteAnimationComponent::GetPlaybackTime() const
{
    return m_playbackTime;
}

bool SpriteAnimationComponent::IsPlaying() const
{
    return m_playbackInfo & PlaybackFlags::Playing;
}

SpriteComponent* SpriteAnimationComponent::GetSpriteComponent() const
{
    return m_spriteComponent;
}
