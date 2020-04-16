/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#pragma once

/*
    Interpolation System
*/

namespace Game
{
    class ComponentSystem;

    class InterpolationSystem final : private NonCopyable, public Resettable<InterpolationSystem>
    {
    public:
        enum class InitializeErrors
        {
            InvalidArgument,
        };

        using InitializeResult = Result<void, InitializeErrors>;

    public:
        InterpolationSystem();
        ~InterpolationSystem();

        InitializeResult Initialize(ComponentSystem* componentSystem);
        void Update(float timeDelta);

    private:
        ComponentSystem* m_componentSystem = nullptr;
        bool m_initialized = false;
    };
}
