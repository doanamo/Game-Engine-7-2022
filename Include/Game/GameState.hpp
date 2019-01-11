/*
    Copyright (c) 2018 Piotr Doan. All rights reserved.
*/

#pragma once

#include "Event/Queue.hpp"
#include "Event/Broker.hpp"
#include "System/UpdateTimer.hpp"
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

        // Pushes an event that will affects the game state.
        // We encapsulate the game state and allow it to mutate only through events.
        void PushEvent(std::any event);

        // Updates the game state.
        // Returns true if the game state was updated.
        bool Update(const System::Timer& timer);

        // Gets the update time.
        float GetUpdateTime() const;

        // Gets the engine reference.
        Engine::Root* GetEngine() const;

    public:
         // Game state events.
        struct Events
        {
            // Sent when game state changes.
            struct GameStateChanged
            {
                GameStateChanged();

                bool stateEntered;
            };

            // Sent when requested to change current update time.
            struct ChangeUpdateTime
            {
                ChangeUpdateTime();

                float updateTime;
            };

            // Called when the class instance is destructed.
            Event::Dispatcher<void()> instanceDestructed;

            // Called when update method is called.
            // This does not mean that the state will be actually updated.
            Event::Dispatcher<void()> updateCalled;

            // Called when state has been updated.
            // Event can be dispatched multiple times during the same update call.
            // This is also a good time to run a custom update logic.
            Event::Dispatcher<void(float)> stateUpdated;
        } events;

    public:
        // Game event system.
        Event::Queue eventQueue;
        Event::Broker eventBroker;

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

        // Event receivers.
        Event::Receiver<bool(const Events::ChangeUpdateTime&)> m_changeUpdateTime;

        // Update parameters.
        float m_updateTime;

        // Initialization state.
        bool m_initialized;
    };
}
