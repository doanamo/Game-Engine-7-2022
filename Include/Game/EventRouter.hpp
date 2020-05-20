/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#pragma once

#include <Event/Receiver.hpp>
#include <Core/ServiceStorage.hpp>
#include <System/InputDefinitions.hpp>
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

    class EventRouter final : private Common::NonCopyable
    {
    public:
        struct CreateFromParams
        {
            const Core::ServiceStorage* services = nullptr;
            GameFramework* gameFramework = nullptr;
        };

        enum class CreateErrors
        {
            InvalidArgument,
            FailedEventSubscription,
        };

        using CreateResult = Common::Result<std::unique_ptr<EventRouter>, CreateErrors>;
        static CreateResult Create(const CreateFromParams& params);

    public:
        ~EventRouter();

        template<typename EventType>
        void PushEvent(const EventType& event);

    private:
        EventRouter();

        template<typename EventType>
        void PushEventReturnVoid(const EventType& event);

        template<typename EventType>
        bool PushEventReturnFalse(const EventType& event);

        GameState* GetCurrentGameState();

    private:
        GameFramework* m_gameFramework = nullptr;

        struct Receivers
        {
            Event::Receiver<bool(const System::InputEvents::KeyboardKey&)> keyboardKeyReceiver;
            Event::Receiver<bool(const System::InputEvents::TextInput&)> textInputReceiver;
            Event::Receiver<bool(const System::InputEvents::MouseButton&)> mouseButtonReceiver;
            Event::Receiver<bool(const System::InputEvents::MouseScroll&)> mouseScrollReceiver;
            Event::Receiver<void(const System::InputEvents::CursorPosition&)> cursorPosition;
            Event::Receiver<void(const System::InputEvents::CursorEnter&)> cursorEnter;
        } m_receivers;
    };

    template<typename EventType>
    void EventRouter::PushEvent(const EventType& event)
    {
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
