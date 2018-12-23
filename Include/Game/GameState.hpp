/*
    Copyright (c) 2018 Piotr Doan. All rights reserved.
*/

#pragma once

#include "System/UpdateTimer.hpp"
#include "Event/Queue.hpp"
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
        bool Update(const System::Timer& timer, Event::Delegate<void(float)> customUpdate = nullptr);

        // Sets the update time.
        void SetUpdateTime(float updateTime);

        // Gets the update time.
        float GetUpdateTime() const;

        // Gets the engine reference.
        Engine::Root* GetEngine() const;

    public:
        // Game state events.
        struct Events
        {
            // Called when the class instance is destructed.
            Event::Dispatcher<void()> instanceDestructed;

            // Called when update method is called.
            Event::Dispatcher<void()> updateCalled;

            // Called when state has been updated.
            // This event can be dispatched multiple
            // times during the same update call.
            Event::Dispatcher<void(float)> stateUpdated;
        } events;

    public:
        // Game event queue.
        Event::Queue eventQueue;

        // Main loop timer.
        System::UpdateTimer updateTimer;

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

        // Update parameters.
        float m_updateTime;

        // Initialization state.
        bool m_initialized;
    };
}
