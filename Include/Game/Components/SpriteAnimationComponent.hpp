/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#pragma once

#include <memory>
#include <Graphics/Sprite/SpriteAnimationList.hpp>
#include "Game/Component.hpp"

/*
    Sprite Animation Component
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

            using Type = unsigned int;
        };

        using SpriteAnimationListPtr = std::shared_ptr<Graphics::SpriteAnimationList>;
        using SpriteAnimation = Graphics::SpriteAnimationList::Animation;

        SpriteAnimationComponent();
        ~SpriteAnimationComponent();

        void SetSpriteAnimationList(SpriteAnimationListPtr spriteAnimationList);
        void ResetInterpolation();

        void Play(std::string animationName, bool loop);
        bool IsPlaying() const;
        void Pause();
        void Resume();
        void Stop();

        void Update(float timeDelta);
        float CalculateAnimationTime(float timeAlpha) const;

        SpriteComponent* GetSpriteComponent() const;
        const SpriteAnimationListPtr& GetSpriteAnimationList() const;
        const SpriteAnimation* GetSpriteAnimation() const;

    private:
        bool OnInitialize(ComponentSystem* componentSystem, const EntityHandle& entitySelf) override;

        SpriteComponent* m_spriteComponent = nullptr;
        SpriteAnimationListPtr m_spriteAnimationList = nullptr;
        const SpriteAnimation* m_spriteAnimation = nullptr;
        PlaybackFlags::Type m_playbackInfo = PlaybackFlags::None;
        float m_currentAnimationTime = 0.0f;
        float m_previousAnimationTime = 0.0f;
    };
}
