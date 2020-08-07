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

    class InterpolationSystem final : private Common::NonCopyable
    {
    public:
        enum class CreateErrors
        {
            InvalidArgument,
        };

        using CreateResult = Common::Result<std::unique_ptr<InterpolationSystem>, CreateErrors>;
        static CreateResult Create(ComponentSystem* componentSystem);

    public:
        ~InterpolationSystem();

        void Tick(float timeDelta);

    private:
        InterpolationSystem();

        ComponentSystem* m_componentSystem = nullptr;
    };
}
