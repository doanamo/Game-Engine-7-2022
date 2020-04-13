/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#pragma once

#include <System/InputDefinitions.hpp>
#include <Event/Receiver.hpp>
#include "Game/GameState.hpp"

namespace System
{
    class InputManager;
}

/*
    Event Router

    Subscribes and listens for important engine events that
    are then routed to the current game state instance.
*/

namespace Game
{
    class GameFramework;
    class GameState;

    class EventRouter : private NonCopyable
    {
    public:
        struct InitializeFromParams
        {
            System::InputManager* inputManager = nullptr;
            GameFramework* gameFramework = nullptr;
        };

    public:
        EventRouter();
        ~EventRouter() = default;

        EventRouter(EventRouter&& other);
        EventRouter& operator=(EventRouter&& other);

        bool Initialize(const InitializeFromParams& params);

        template<typename EventType>
        void PushEvent(const EventType& event);

    private:
        template<typename EventType>
        void PushEventReturnVoid(const EventType& event);

        template<typename EventType>
        bool PushEventReturnFalse(const EventType& event);

        GameState* GetCurrentGameState();

    private:
        struct Receivers
        {
            Event::Receiver<bool(const System::InputEvents::KeyboardKey&)> keyboardKeyReceiver;
            Event::Receiver<bool(const System::InputEvents::TextInput&)> textInputReceiver;
            Event::Receiver<bool(const System::InputEvents::MouseButton&)> mouseButtonReceiver;
            Event::Receiver<bool(const System::InputEvents::MouseScroll&)> mouseScrollReceiver;
            Event::Receiver<void(const System::InputEvents::CursorPosition&)> cursorPosition;
            Event::Receiver<void(const System::InputEvents::CursorEnter&)> cursorEnter;
        } m_receivers;

    private:
        GameFramework* m_gameFramework = nullptr;
        bool m_initialized = false;
    };

    template<typename EventType>
    void EventRouter::PushEvent(const EventType& event)
    {
        ASSERT(m_initialized, "Event listener is not initialized!");

        // Push event to current game state.
        GameState* gameState = this->GetCurrentGameState();

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
        return;
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
