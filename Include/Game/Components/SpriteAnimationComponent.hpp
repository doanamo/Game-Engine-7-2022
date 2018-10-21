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

        // Animation playback info.
        const SpriteAnimation* m_spriteAnimation;

        float m_frameTime;
        std::size_t m_frameIndex;
        bool m_frameChanged;

        float m_playbackTime;
        bool m_playing;
        bool m_loop;
    };
}
