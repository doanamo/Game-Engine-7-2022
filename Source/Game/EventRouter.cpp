/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#include "Game/EventRouter.hpp"
#include "Game/GameFramework.hpp"
#include "Game/GameState.hpp"
#include <System/InputManager.hpp>
using namespace Game;

EventRouter::EventRouter()
{
    // Bind event listeners.
    m_receivers.keyboardKeyReceiver.Bind<EventRouter, &EventRouter::PushEventReturnFalse<System::InputEvents::KeyboardKey>>(this);
    m_receivers.textInputReceiver.Bind<EventRouter, &EventRouter::PushEventReturnFalse<System::InputEvents::TextInput>>(this);
    m_receivers.mouseButtonReceiver.Bind<EventRouter, &EventRouter::PushEventReturnFalse<System::InputEvents::MouseButton>>(this);
    m_receivers.mouseScrollReceiver.Bind<EventRouter, &EventRouter::PushEventReturnFalse<System::InputEvents::MouseScroll>>(this);
    m_receivers.cursorPosition.Bind<EventRouter, &EventRouter::PushEventReturnVoid<System::InputEvents::CursorPosition>>(this);
    m_receivers.cursorEnter.Bind<EventRouter, &EventRouter::PushEventReturnVoid<System::InputEvents::CursorEnter>>(this);
}

EventRouter::EventRouter(EventRouter&& other) :
    EventRouter()
{
    *this = std::move(other);
}

EventRouter& EventRouter::operator=(EventRouter&& other)
{
    std::swap(m_gameFramework, other.m_gameFramework);
    std::swap(m_receivers, other.m_receivers);
    std::swap(m_initialized, other.m_initialized);

    return *this;
}

bool EventRouter::Initialize(const InitializeFromParams& params)
{
    LOG("Initializing event router...");
    LOG_SCOPED_INDENT();

    // Ensure that event router has not been initialized yet.
    ASSERT(!m_initialized, "Event router has already been initialized!");

    // Create an initialization guard.
    SCOPE_GUARD_IF(!m_initialized, *this = EventRouter());

    // Validate arguments.
    if(params.inputManager == nullptr)
    {
        LOG_ERROR("Invalid argument - \"inputManager\" is null!");
        return false;
    }

    if(params.gameFramework == nullptr)
    {
        LOG_ERROR("Invalid argument - \"gameFramework\" is null!");
        return false;
    }

    m_gameFramework = params.gameFramework;

    // Subscribe event receivers.
    bool subscriptionResult = true;

    subscriptionResult &= m_receivers.keyboardKeyReceiver.Subscribe(params.inputManager->events.keyboardKey);
    subscriptionResult &= m_receivers.textInputReceiver.Subscribe(params.inputManager->events.textInput);
    subscriptionResult &= m_receivers.mouseButtonReceiver.Subscribe(params.inputManager->events.mouseButton);
    subscriptionResult &= m_receivers.mouseScrollReceiver.Subscribe(params.inputManager->events.mouseScroll);
    subscriptionResult &= m_receivers.cursorPosition.Subscribe(params.inputManager->events.cursorPosition);
    subscriptionResult &= m_receivers.cursorEnter.Subscribe(params.inputManager->events.cursorEnter);

    if(!subscriptionResult)
    {
        LOG_ERROR("Could not subscribe event receivers!");
        return false;
    }

    // Success!
    return m_initialized = true;
}

GameState* EventRouter::GetCurrentGameState()
{
    return m_gameFramework->GetGameState().get();
}
