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
    class SpriteSystem final : private NonCopyable, public Resettable<SpriteSystem>
    {
    public:
        enum class InitializeErrors
        {
            InvalidArgument,
        };

        using InitializeResult = Result<void, InitializeErrors>;

    public:
        SpriteSystem();
        ~SpriteSystem();

        InitializeResult Initialize(ComponentSystem* componentSystem);
        void Update(float timeDelta);

    private:
        ComponentSystem* m_componentSystem = nullptr;
        bool m_initialized = false;
    };
}
