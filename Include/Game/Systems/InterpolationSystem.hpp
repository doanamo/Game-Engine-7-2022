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

    class InterpolationSystem final : private NonCopyable
    {
    public:
        enum class CreateErrors
        {
            InvalidArgument,
        };

        using CreateResult = Result<std::unique_ptr<InterpolationSystem>, CreateErrors>;
        static CreateResult Create(ComponentSystem* componentSystem);

    public:
        ~InterpolationSystem();

        void Update(float timeDelta);

    private:
        InterpolationSystem();

    private:
        ComponentSystem* m_componentSystem = nullptr;
    };
}
