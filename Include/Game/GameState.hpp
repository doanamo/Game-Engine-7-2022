/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#pragma once

#include <Common/StateMachine.hpp>

namespace Game
{
    class TickTimer;
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

        virtual void Update(const float timeDelta) = 0;
        virtual void Tick(const float tickTime) = 0;
        virtual void Draw(const float timeAlpha) = 0;

        // Override if game state wants to control how tick is called.
        // Without it tick method is always called along update.
        virtual TickTimer* GetTickTimer() const
        {
            return nullptr;
        }

        // Override if game state provides game instance.
        // Game framework will then automatically process game instance.
        virtual GameInstance* GetGameInstance() const
        {
            return nullptr;
        }
    };
}
