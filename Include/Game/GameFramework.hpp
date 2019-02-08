/*
    Copyright (c) 2018-2019 Piotr Doan. All rights reserved.
*/

#pragma once

#include "Game/EventRouter.hpp"

// Forward declarations.
namespace Engine
{
    class Root;
}

/*
    Game Framework
*/

namespace Game
{
    // Forward declarations.
    class GameState;

    // Game framework class.
    class GameFramework
    {
    public:
        GameFramework();
        ~GameFramework();

        // Disallow copying.
        GameFramework(const GameFramework& other);
        GameFramework& operator=(const GameFramework& other);

        // Move constructor and assignment.
        GameFramework(GameFramework&& other);
        GameFramework& operator=(GameFramework&& other);

        // Initializes game framework.
        bool Initialize(Engine::Root* engine);

        // Updates game framework.
        void Update();

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
        // Engine reference.
        Engine::Root* m_engine;

        // Game event router.
        EventRouter m_eventRouter;

        // Current game state.
        std::shared_ptr<GameState> m_gameState;

        // Initialization state.
        bool m_initialized;
    };
}
