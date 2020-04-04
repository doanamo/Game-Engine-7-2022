/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#pragma once

#include "Game/EventRouter.hpp"

// Forward declarations.
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
    // Forward declarations.
    class GameState;

    // Game framework class.
    class GameFramework : private NonCopyable
    {
    private:
        // Initialization from parameters.
        struct InitializeFromParams
        {
            System::Timer* timer = nullptr;
            System::Window* window = nullptr;
            System::InputManager* inputManager = nullptr;
            Renderer::StateRenderer* stateRenderer = nullptr;
        };

    public:
        GameFramework() = default;
        ~GameFramework() = default;

        GameFramework(GameFramework&& other);
        GameFramework& operator=(GameFramework&& other);

        // Initializes game framework.
        bool Initialize(const InitializeFromParams& params);

        // Updates game framework.
        // Returns true if game state actually updates.
        bool Update();

        // Draws game framework.
        void Draw();

        // Sets the main game state instance.
        void SetGameState(std::shared_ptr<GameState>& gameState);

        // Gets the current game state instance.
        std::shared_ptr<GameState> GetGameState();

    public:
        // Framework events.
        struct Events
        {
            // Called when current game state changes.
            Event::Dispatcher<void(const std::shared_ptr<GameState>&)> gameStateChanged;
        } events;

    private:
        // System references.
        System::Timer* m_timer = nullptr;
        System::Window* m_window = nullptr;
        Renderer::StateRenderer* m_stateRenderer = nullptr;

        // Engine event router.
        EventRouter m_eventRouter;

        // Current game state.
        std::shared_ptr<GameState> m_gameState;

        // Initialization state.
        bool m_initialized = false;
    };
}
