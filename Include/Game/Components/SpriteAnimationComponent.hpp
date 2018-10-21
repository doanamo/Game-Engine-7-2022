/*
    Copyright (c) 2018 Piotr Doan. All rights reserved.
*/

#pragma once

#include "Game/Component.hpp"
#include "Graphics/Sprites/SpriteAnimationList.hpp"

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

                FirstFrame = 1 << 3,
                FrameChanged = 1 << 4,
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

        // Plays a specific sprite animation.
        void Play(std::string animationName, bool loop);

        // Pauses the currently playing animation.
        void Pause();

        // Resumes the currently paused animation.
        void Resume();

        // Stops the currently playing animation.
        void Stop();

        // Updates the currently playing animation.
        void Update(float timeDelta);

        // Gets the sprite animation list.
        const SpriteAnimationListPtr& GetSpriteAnimationList() const;

        // Gets the current sprite animation.
        const SpriteAnimation* GetSpriteAnimation() const;

        // Gets the current frame index.
        std::size_t GetFrameIndex() const;

        // Checks if animation frame has changed.
        bool HasFrameChanged() const;

        // Gets the current playback time.
        float GetPlaybackTime() const;

        // Checks if sprite animation is being played.
        bool IsPlaying() const;

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

        // Current sprite animation.
        const SpriteAnimation* m_spriteAnimation;

        // Animation frame info.
        std::size_t m_frameIndex;
        float m_frameTime;

        // Animation playback info.
        PlaybackFlags::Type m_playbackInfo;
        float m_playbackTime;
    };
}
