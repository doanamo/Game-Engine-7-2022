/*
    Copyright (c) 2018 Piotr Doan. All rights reserved.
*/

#pragma once

#include "System/Timer.hpp"
#include "Game/EntitySystem.hpp"
#include "Game/ComponentSystem.hpp"
#include "Game/Systems/IdentitySystem.hpp"
#include "Game/Systems/InterpolationSystem.hpp"
#include "Game/Systems/SpriteSystem.hpp"

// Forward declaration.
namespace Engine
{
    class Root;
}

/*
    Game State
*/

namespace Game
{
    // Game state class.
    class GameState
    {
    public:
        GameState();
        ~GameState();

        GameState(const GameState& other) = delete;
        GameState& operator=(const GameState& other) = delete;

        GameState(GameState&& other);
        GameState& operator=(GameState&& other);

        // Initializes the game state.
        bool Initialize(Engine::Root* engine);

        // Updates the game state.
        // Returns true if the game state was updated.
        bool Update(const System::Timer& timer);

        // Gets the engine reference.
        Engine::Root* GetEngine() const;

    public:
        // Main loop timer.
        System::Timer timer;

        // Core game systems.
        Game::EntitySystem entitySystem;
        Game::ComponentSystem componentSystem;

        // Base game systems.
        Game::IdentitySystem identitySystem;
        Game::InterpolationSystem interpolationSystem;
        Game::SpriteSystem spriteSystem;

    private:
        // Engine reference.
        Engine::Root* m_engine;

        // Initialization state.
        bool m_initialized;
    };
}
