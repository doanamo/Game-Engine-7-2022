/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#pragma once

#include <any>
#include <Event/Queue.hpp>
#include <Event/Broker.hpp>
#include "Game/UpdateTimer.hpp"
#include "Game/EntitySystem.hpp"
#include "Game/ComponentSystem.hpp"
#include "Game/Systems/IdentitySystem.hpp"
#include "Game/Systems/InterpolationSystem.hpp"
#include "Game/Systems/SpriteSystem.hpp"

/*
    Game State
*/

namespace Game
{
    // Game state class.
    class GameState : private NonCopyable
    {
    public:
        GameState();
        ~GameState();

        GameState(GameState&& other);
        GameState& operator=(GameState&& other);

        // Initializes the game state.
        bool Initialize();

        // Pushes an event that will affects the game state.
        // We encapsulate the game state and allow it to mutate only through events.
        void PushEvent(std::any event);

        // Updates the game state.
        // Returns true if the game state was updated.
        bool Update(const System::Timer& timer);

        // Gets the update time.
        float GetUpdateTime() const;

    public:
         // Game state events.
        struct Events
        {
            // Sent when game state changes.
            struct GameStateChanged
            {
                bool stateEntered = false;
            };

            // Sent when requested to change current update time.
            struct ChangeUpdateTime
            {
                float updateTime = 0.0f;
            };

            // Called when the class instance is destructed.
            Event::Dispatcher<void()> instanceDestructed;

            // Called when update method is called.
            // This does not mean that the state will be actually updated.
            Event::Dispatcher<void()> updateCalled;

            // Called when state had its update processed.
            // Event can be dispatched multiple times during the same update call.
            // This is also a good time to run a custom update logic.
            Event::Dispatcher<void(float)> updateProcessed;
        } events;

    public:
        // Game event system.
        Event::Queue eventQueue;
        Event::Broker eventBroker;

        // Main loop timer.
        UpdateTimer updateTimer;

        // Core game systems.
        EntitySystem entitySystem;
        ComponentSystem componentSystem;

        // Base game systems.
        IdentitySystem identitySystem;
        InterpolationSystem interpolationSystem;
        SpriteSystem spriteSystem;

    private:
        // Event receivers.
        Event::Receiver<bool(const Events::ChangeUpdateTime&)> m_changeUpdateTime;

        // Update parameters.
        float m_updateTime = 1.0f / 10.0f;

        // Initialization state.
        bool m_initialized = false;
    };
}
