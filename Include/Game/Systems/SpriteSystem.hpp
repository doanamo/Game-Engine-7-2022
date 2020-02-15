/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
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
    // Sprite system class.
    class SpriteSystem
    {
    public:
        SpriteSystem();
        ~SpriteSystem();

        SpriteSystem(const SpriteSystem& other) = delete;
        SpriteSystem& operator=(const SpriteSystem& other) = delete;

        SpriteSystem(SpriteSystem&& other);
        SpriteSystem& operator=(SpriteSystem&& other);

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
