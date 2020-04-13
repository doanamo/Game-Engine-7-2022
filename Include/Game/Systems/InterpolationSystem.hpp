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

    class InterpolationSystem : private NonCopyable
    {
    public:
        InterpolationSystem() = default;
        ~InterpolationSystem() = default;

        InterpolationSystem(InterpolationSystem&& other);
        InterpolationSystem& operator=(InterpolationSystem&& other);

        bool Initialize(ComponentSystem* componentSystem);
        void Update(float timeDelta);

    private:
        ComponentSystem* m_componentSystem = nullptr;
        bool m_initialized = false;
    };
}
