/*
    Copyright (c) 2018 Piotr Doan. All rights reserved.
*/

#pragma once

#include <System/Window.hpp>

// Forward declarations.
namespace Engine
{
    class Root;
}

/*
    Event Router

    Subscribes and listens for important engine events that
    are then routed to the current game state instance.
*/

namespace Game
{
    // Event router class.
    class EventRouter
    {
    public:
        EventRouter();
        ~EventRouter();

        EventRouter(const EventRouter& other) = delete;
        EventRouter& operator=(const EventRouter& other) = delete;

        EventRouter(EventRouter&& other);
        EventRouter& operator=(EventRouter&& other);

        // Initializes the event router instance.
        bool Initialize(Engine::Root* engine);

        // Pushes event to current game state.
        template<typename EventType>
        void PushEvent(const EventType& event);

    private:
        // Pushes event to game state and returns void.
        template<typename EventType>
        void PushEventReturnVoid(const EventType& event);

        // Pushes event to game state and returns false.
        template<typename EventType>
        bool PushEventReturnFalse(const EventType& event);

    private:
        // Engine reference.
        Engine::Root* m_engine;

        // Window event receivers.
        struct Events
        {
            // Input events.
            Event::Receiver<bool(const System::Window::Events::KeyboardKey&)> keyboardKeyReceiver;
            Event::Receiver<bool(const System::Window::Events::TextInput&)> textInputReceiver;
            Event::Receiver<bool(const System::Window::Events::MouseButton&)> mouseButtonReceiver;
            Event::Receiver<bool(const System::Window::Events::MouseScroll&)> mouseScrollReceiver;
            Event::Receiver<void(const System::Window::Events::CursorPosition&)> cursorPosition;
        } m_events;

        // Initialization state.
        bool m_initialized;
    };

    // Template definitions.
    template<typename EventType>
    void EventRouter::PushEvent(const EventType& event)
    {
        ASSERT(m_initialized, "Event listener is not initialized!");

        // Push event to current game state.
        auto gameState = m_engine->GetGameState();

        if(gameState != nullptr)
        {
            gameState->PushEvent(event);
        }
    }

    template<typename EventType>
    void EventRouter::PushEventReturnVoid(const EventType& event)
    {
        // Push event to current game state.
        this->PushEvent(event);

        // Return nothing.
        return void;
    }

    template<typename EventType>
    bool EventRouter::PushEventReturnFalse(const EventType& event)
    {
        // Push event to current game state.
        this->PushEvent(event);

        // In case of window events, returning false will
        // make them continue to propagate further.
        return false;
    }
}
