/*
    Copyright (c) 2018 Piotr Doan. All rights reserved.
*/

#pragma once

// Forward declarations.
namespace Game
{
    class ComponentSystem;
}

/*
    Sprite Animation System
*/

namespace Game
{
    // Sprite animation system class.
    class SpriteAnimationSystem
    {
    public:
        SpriteAnimationSystem();
        ~SpriteAnimationSystem();

        SpriteAnimationSystem(const SpriteAnimationSystem& other) = delete;
        SpriteAnimationSystem& operator=(const SpriteAnimationSystem& other) = delete;

        SpriteAnimationSystem(SpriteAnimationSystem&& other);
        SpriteAnimationSystem& operator=(SpriteAnimationSystem&& other);

        // Initializes the sprite animation system.
        bool Initialize(ComponentSystem* componentSystem);

        // Updates the sprite animation system.
        void Update(float timeDelta);

    private:
        // Component system reference.
        ComponentSystem* m_componentSystem;

        // Initialization state.
        bool m_initialized;
    };
}
