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
        using SpriteAnimationPtr = const Graphics::SpriteAnimationList::Animation*;

    public:
        SpriteAnimationComponent();

        SpriteAnimationComponent(SpriteAnimationComponent&& other);
        SpriteAnimationComponent& operator=(SpriteAnimationComponent&& other);

        // Plays a specific sprite animation.
        void Play(std::string animationName, bool loop);

        // Sets the sprite animation list.
        void SetSpriteAnimationList(SpriteAnimationListPtr spriteAnimationList);

        // Gets the sprite animation list.
        const SpriteAnimationListPtr& GetSpriteAnimationList() const;

    private:
        // Called when component gets initialized.
        bool OnInitialize(ComponentSystem* componentSystem, const EntityHandle& entitySelf) override;

        // Called when component is updated.
        void OnUpdate(float timeDelta);

    private:
        // Sprite component reference.
        SpriteComponent* m_spriteComponent;

        // Sprite animation list.
        SpriteAnimationListPtr m_spriteAnimationList;

        // Animation playback info.
        SpriteAnimationPtr m_spriteAnimation;
        float m_currentTime;
        bool m_playing;
        bool m_loop;
    };
}
