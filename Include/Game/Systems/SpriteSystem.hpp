/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#pragma once

namespace Game
{
    class ComponentSystem;
}

/*
    Sprite System
*/

namespace Game
{
    class SpriteSystem final : private Common::NonCopyable
    {
    public:
        enum class CreateErrors
        {
            InvalidArgument,
        };

        using CreateResult = Common::Result<std::unique_ptr<SpriteSystem>, CreateErrors>;
        static CreateResult Create(ComponentSystem* componentSystem);

    public:
        ~SpriteSystem();

        void Tick(float timeDelta);

    private:
        SpriteSystem();

        ComponentSystem* m_componentSystem = nullptr;
    };
}
