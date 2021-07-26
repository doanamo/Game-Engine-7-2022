/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#pragma once

#include "Game/Component.hpp"
#include <Graphics/Sprite/SpriteAnimationList.hpp>

/*
    Sprite Animation Component

    Playback control for animated sequence of sprites.
*/

namespace Game
{
    class SpriteComponent;

    class SpriteAnimationComponent final : public Component
    {
    public:
        struct PlaybackFlags
        {
            enum
            {
                None = 0 << 0,
                Playing = 1 << 0,
                Loop = 1 << 2,
            };

            using Type = uint8_t;
        };

        using SpriteAnimationListPtr = std::shared_ptr<Graphics::SpriteAnimationList>;
        using SpriteAnimation = Graphics::SpriteAnimationList::Animation;

    public:
        SpriteAnimationComponent();
        ~SpriteAnimationComponent();

        void SetSpriteAnimationList(SpriteAnimationListPtr spriteAnimationList);
        void ResetInterpolation();
        void Tick(float timeDelta);

        void Play(std::string animationName, bool loop);
        void Pause();
        void Resume();
        void Stop();

        float CalculateAnimationTime(float timeAlpha) const;

        bool IsPlaying() const
        {
            return m_playbackInfo & PlaybackFlags::Playing;
        }

        bool IsLooped() const
        {
            return m_playbackInfo & PlaybackFlags::Loop;
        }

        SpriteComponent* GetSpriteComponent() const
        {
            ASSERT(m_spriteComponent);
            return m_spriteComponent;
        }

        const SpriteAnimationListPtr& GetSpriteAnimationList() const
        {
            return m_spriteAnimationList;
        }

        const SpriteAnimation* GetCurrentSpriteAnimation() const
        {
            return m_currentSpriteAnimation;
        }

    private:
        bool OnInitialize(ComponentSystem* componentSystem,
            const EntityHandle& entitySelf) override;

    private:
        SpriteComponent* m_spriteComponent = nullptr;
        SpriteAnimationListPtr m_spriteAnimationList = nullptr;
        const SpriteAnimation* m_currentSpriteAnimation = nullptr;
        PlaybackFlags::Type m_playbackInfo = PlaybackFlags::None;
        float m_currentAnimationTime = 0.0f;
        float m_previousAnimationTime = 0.0f;
    };
}
