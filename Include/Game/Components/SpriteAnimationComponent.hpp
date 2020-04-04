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
    // Forward declarations.
    class SpriteComponent;

    // Sprite animation component class.
    class SpriteAnimationComponent : public Component
    {
    public:
        // Playback flags.
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

        // Type declarations.
        using SpriteAnimationListPtr = std::shared_ptr<Graphics::SpriteAnimationList>;
        using SpriteAnimation = Graphics::SpriteAnimationList::Animation;

    public:
        SpriteAnimationComponent();

        SpriteAnimationComponent(SpriteAnimationComponent&& other);
        SpriteAnimationComponent& operator=(SpriteAnimationComponent&& other);

        // Sets the sprite animation list.
        void SetSpriteAnimationList(SpriteAnimationListPtr spriteAnimationList);

        // Resets the animation interpolation.
        void ResetInterpolation();

        // Plays a specific sprite animation.
        void Play(std::string animationName, bool loop);

        // Checks if sprite animation is being played.
        bool IsPlaying() const;

        // Pauses the currently playing animation.
        void Pause();

        // Resumes the currently paused animation.
        void Resume();

        // Stops the currently playing animation.
        void Stop();

        // Updates the currently playing animation.
        void Update(float timeDelta);

        // Calculates the current animation time.
        float CalculateAnimationTime(float timeAlpha) const;

        // Gets the sprite animation list.
        const SpriteAnimationListPtr& GetSpriteAnimationList() const;

        // Gets the current sprite animation.
        const SpriteAnimation* GetSpriteAnimation() const;

        // Gets the sprite component.
        SpriteComponent* GetSpriteComponent() const;

    private:
        // Called when component gets initialized.
        bool OnInitialize(ComponentSystem* componentSystem, const EntityHandle& entitySelf) override;

    private:
        // Sprite component reference.
        SpriteComponent* m_spriteComponent;

        // Sprite animation list.
        SpriteAnimationListPtr m_spriteAnimationList;

        // Animation playback info.
        const SpriteAnimation* m_spriteAnimation;
        PlaybackFlags::Type m_playbackInfo;

        // Interpolated animation time.
        float m_currentAnimationTime;
        float m_previousAnimationTime;
    };
}
