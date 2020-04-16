/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#pragma once

#include "Game/EventRouter.hpp"

namespace System
{
    class Timer;
    class Window;
    class InputManager;
}

namespace Renderer
{
    class StateRenderer;
}

/*
    Game Framework
*/

namespace Game
{
    class GameState;

    class GameFramework final : private NonCopyable, public Resettable<GameFramework>
    {
    public:
        enum class InitializeErrors
        {
            InvalidArgument,
            FailedEventRouterInitialization,
        };

        using InitializeResult = Result<void, InitializeErrors>;

        struct InitializeFromParams
        {
            System::Timer* timer = nullptr;
            System::Window* window = nullptr;
            System::InputManager* inputManager = nullptr;
            Renderer::StateRenderer* stateRenderer = nullptr;
        };

    public:
        GameFramework();
        ~GameFramework();

        InitializeResult Initialize(const InitializeFromParams& params);
        bool Update();
        void Draw();

        void SetGameState(std::shared_ptr<GameState>& gameState);
        std::shared_ptr<GameState> GetGameState();

    public:
        struct Events
        {
            Event::Dispatcher<void(const std::shared_ptr<GameState>&)> gameStateChanged;
        } events;

    private:
        System::Timer* m_timer = nullptr;
        System::Window* m_window = nullptr;
        Renderer::StateRenderer* m_stateRenderer = nullptr;

        EventRouter m_eventRouter;
        std::shared_ptr<GameState> m_gameState;

        bool m_initialized = false;
    };
}
