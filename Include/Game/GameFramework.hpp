/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#pragma once

#include <Common/Event/EventDispatcher.hpp>
#include <Core/EngineSystem.hpp>
#include "Game/GameState.hpp"

namespace System
{
    class Timer;
    class Window;
    class InputManager;
}

namespace Renderer
{
    class GameRenderer;
}

/*
    Game Framework
*/

namespace Game
{
    class GameState;

    class GameFramework final : public Core::EngineSystem
    {
        REFLECTION_ENABLE(GameFramework, Core::EngineSystem)

    public:
        enum class ChangeGameStateErrors
        {
            AlreadyCurrent,
            FailedTransition,
        };

        using ChangeGameStateResult = Common::Result<void, ChangeGameStateErrors>;

    public:
        GameFramework();
        ~GameFramework() override;

        ChangeGameStateResult ChangeGameState(std::shared_ptr<GameState> gameState);
        void ProcessGameState();
        bool HasGameState() const;

        struct Events
        {
            // Called whether game state changes.
            // This will be dispatched only on successful game state transition.
            Event::Dispatcher<void(const std::shared_ptr<GameState>&)> gameStateChanged;

            // Called when tick method is called.
            // This does not mean that the state was actually ticked.
            Event::Dispatcher<void()> tickRequested;

            // Called when state had its tick processed.
            // Event can be dispatched multiple times during the same tick method call.
            // This is also good time to run custom tick logic in response.
            Event::Dispatcher<void(float)> tickProcessed;

            // Called when state had its update processed.
            // Event will be dispatched only once event if multiple ticks are processed.
            // This is also good time to run custom update logic in response.
            Event::Dispatcher<void(float)> updateProcessed;

            // Called when game instance should be drawn, before game state's custom draw.
            Event::Dispatcher<void(GameInstance*, float)> drawGameInstance;
        } events;

    private:
        bool OnAttach(const Core::EngineSystemStorage& engineSystems) override;
        bool OnFinalize(const Core::EngineSystemStorage& engineSystems) override;
        void OnProcessFrame() override;

    private:
        System::Timer* m_timer = nullptr;
        Common::StateMachine<GameState> m_stateMachine;
    };
}

REFLECTION_TYPE(Game::GameFramework, Core::EngineSystem)
