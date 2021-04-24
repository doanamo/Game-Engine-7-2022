/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#pragma once

/*
    Game System
*/

namespace Game
{
    class GameInstance;

    class GameSystem : private Common::NonCopyable
    {
        REFLECTION_ENABLE(GameSystem)

    public:
        virtual ~GameSystem() = default;

    protected:
        friend GameInstance;

        virtual bool OnAttach(GameInstance* gameInstance)
        {
            return true;
        }

        virtual bool OnFinalize(GameInstance* gameInstance)
        {
            return true;
        }

        virtual void OnTick(float timeDelta)
        {
        }
    };
}

REFLECTION_TYPE(Game::GameSystem)
