/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
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

        void Update(float timeDelta);

    private:
        SpriteSystem();

    private:
        ComponentSystem* m_componentSystem = nullptr;
    };
}
