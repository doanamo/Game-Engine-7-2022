/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#pragma once

#include <Common/StateMachine.hpp>

namespace Game
{
    class GameInstance;
}

/*
    Game State
*/

namespace Game
{
    class GameState : public Common::State<GameState>, private Common::NonCopyable
    {
    public:
        virtual ~GameState() = default;

        virtual void Update(float timeDelta) = 0;
        virtual void Tick(float timeDelta) = 0;
        virtual void Draw(float timeAlpha) = 0;

        virtual GameInstance* GetGameInstance() const
        {
            return nullptr;
        }
    };
}
