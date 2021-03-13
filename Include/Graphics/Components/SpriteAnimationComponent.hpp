/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#pragma once

#include <Game/Component.hpp>
#include "Graphics/Sprite/SpriteAnimationList.hpp"

/*
    Sprite Animation Component

    Playback control for animated sequence of sprites.
*/

namespace Graphics
{
    class SpriteComponent;

    class SpriteAnimationComponent final : public Game::Component
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

            using Type = unsigned int;
        };

        using SpriteAnimationListPtr = std::shared_ptr<Graphics::SpriteAnimationList>;
        using SpriteAnimation = Graphics::SpriteAnimationList::Animation;

        SpriteAnimationComponent();
        ~SpriteAnimationComponent();

        void SetSpriteAnimationList(SpriteAnimationListPtr spriteAnimationList);
        void ResetInterpolation();
        void Tick(float timeDelta);

        void Play(std::string animationName, bool loop);
        bool IsPlaying() const;
        bool IsLooped() const;
        void Pause();
        void Resume();
        void Stop();

        float CalculateAnimationTime(float timeAlpha) const;
        SpriteComponent* GetSpriteComponent() const;
        const SpriteAnimationListPtr& GetSpriteAnimationList() const;
        const SpriteAnimation* GetSpriteAnimation() const;

    private:
        bool OnInitialize(Game::ComponentSystem* componentSystem,
            const Game::EntityHandle& entitySelf) override;

        SpriteComponent* m_spriteComponent = nullptr;
        SpriteAnimationListPtr m_spriteAnimationList = nullptr;
        const SpriteAnimation* m_playingSpriteAnimation = nullptr;
        PlaybackFlags::Type m_playbackInfo = PlaybackFlags::None;
        float m_currentAnimationTime = 0.0f;
        float m_previousAnimationTime = 0.0f;
    };
}
