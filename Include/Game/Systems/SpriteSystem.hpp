/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#pragma once

namespace Game
{
    class ComponentSystem;
}

/*
    Sprite Animation System
*/

namespace Game
{
    class SpriteSystem : private NonCopyable
    {
    public:
        SpriteSystem() = default;
        ~SpriteSystem() = default;

        SpriteSystem(SpriteSystem&& other);
        SpriteSystem& operator=(SpriteSystem&& other);

        bool Initialize(ComponentSystem* componentSystem);
        void Update(float timeDelta);

    private:
        ComponentSystem* m_componentSystem = nullptr;
        bool m_initialized = false;
    };
}
