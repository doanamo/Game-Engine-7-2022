/*
    Copyright (c) 2018-2019 Piotr Doan. All rights reserved.
*/

#include "Precompiled.hpp"
#include "Game/EventRouter.hpp"
#include "Game/GameState.hpp"
#include "System/Window.hpp"
#include "Engine/Root.hpp"
using namespace Game;

EventRouter::EventRouter() :
    m_engine(nullptr),
    m_initialized(false)
{
    // Bind event listeners.
    m_events.keyboardKeyReceiver.Bind<EventRouter, &EventRouter::PushEventReturnFalse<System::Window::Events::KeyboardKey>>(this);
    m_events.textInputReceiver.Bind<EventRouter, &EventRouter::PushEventReturnFalse<System::Window::Events::TextInput>>(this);
    m_events.mouseButtonReceiver.Bind<EventRouter, &EventRouter::PushEventReturnFalse<System::Window::Events::MouseButton>>(this);
    m_events.mouseScrollReceiver.Bind<EventRouter, &EventRouter::PushEventReturnFalse<System::Window::Events::MouseScroll>>(this);
    m_events.cursorPosition.Bind<EventRouter, &EventRouter::PushEventReturnVoid<System::Window::Events::CursorPosition>>(this);
}

EventRouter::~EventRouter()
{
}

EventRouter::EventRouter(EventRouter&& other) :
    EventRouter()
{
    *this = std::move(other);
}

EventRouter& EventRouter::operator=(EventRouter&& other)
{
    std::swap(m_engine, other.m_engine);
    std::swap(m_events, other.m_events);
    std::swap(m_initialized, other.m_initialized);

    return *this;
}

bool EventRouter::Initialize(Engine::Root* engine)
{
    LOG() << "Initializing event router..." << LOG_INDENT();

    // Ensure that event router has not been initialized yet.
    ASSERT(!m_initialized, "Event router has already been initialized!");

    // Create an initialization guard.
    SCOPE_GUARD_IF(!m_initialized, *this = EventRouter());

    // Save engine reference.
    if(engine == nullptr)
    {
        LOG_ERROR() << "Invalid argument - \"engine\" is null!";
        return false;
    }

    m_engine = engine;

    // Get window instance.
    System::Window& window = m_engine->GetWindow();

    // Subscribe event receivers.
    bool subscriptionResult = true;

    subscriptionResult &= m_events.keyboardKeyReceiver.Subscribe(window.events.keyboardKey);
    subscriptionResult &= m_events.textInputReceiver.Subscribe(window.events.textInput);
    subscriptionResult &= m_events.mouseButtonReceiver.Subscribe(window.events.mouseButton);
    subscriptionResult &= m_events.mouseScrollReceiver.Subscribe(window.events.mouseScroll);
    subscriptionResult &= m_events.cursorPosition.Subscribe(window.events.cursorPosition);

    if(!subscriptionResult)
    {
        LOG_ERROR() << "Could not subscribe event receivers!";
        return false;
    }

    // Success!
    return m_initialized = true;
}
