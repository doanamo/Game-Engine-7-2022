/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#include "Precompiled.hpp"
#include "Game/EventRouter.hpp"
#include "Game/GameFramework.hpp"
#include "System/InputManager.hpp"
#include "Engine/Root.hpp"
using namespace Game;

EventRouter::EventRouter() :
    m_engine(nullptr),
    m_initialized(false)
{
    // Bind event listeners.
    m_receivers.keyboardKeyReceiver.Bind<EventRouter, &EventRouter::PushEventReturnFalse<System::InputEvents::KeyboardKey>>(this);
    m_receivers.textInputReceiver.Bind<EventRouter, &EventRouter::PushEventReturnFalse<System::InputEvents::TextInput>>(this);
    m_receivers.mouseButtonReceiver.Bind<EventRouter, &EventRouter::PushEventReturnFalse<System::InputEvents::MouseButton>>(this);
    m_receivers.mouseScrollReceiver.Bind<EventRouter, &EventRouter::PushEventReturnFalse<System::InputEvents::MouseScroll>>(this);
    m_receivers.cursorPosition.Bind<EventRouter, &EventRouter::PushEventReturnVoid<System::InputEvents::CursorPosition>>(this);
    m_receivers.cursorEnter.Bind<EventRouter, &EventRouter::PushEventReturnVoid<System::InputEvents::CursorEnter>>(this);
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
    std::swap(m_receivers, other.m_receivers);
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

    // Get input manager instance.
    System::InputManager& inputManager = engine->GetInputManager();

    // Subscribe event receivers.
    bool subscriptionResult = true;

    subscriptionResult &= m_receivers.keyboardKeyReceiver.Subscribe(inputManager.events.keyboardKey);
    subscriptionResult &= m_receivers.textInputReceiver.Subscribe(inputManager.events.textInput);
    subscriptionResult &= m_receivers.mouseButtonReceiver.Subscribe(inputManager.events.mouseButton);
    subscriptionResult &= m_receivers.mouseScrollReceiver.Subscribe(inputManager.events.mouseScroll);
    subscriptionResult &= m_receivers.cursorPosition.Subscribe(inputManager.events.cursorPosition);
    subscriptionResult &= m_receivers.cursorEnter.Subscribe(inputManager.events.cursorEnter);

    if(!subscriptionResult)
    {
        LOG_ERROR() << "Could not subscribe event receivers!";
        return false;
    }

    // Success!
    return m_initialized = true;
}

GameState* EventRouter::GetCurrentGameState()
{
    return m_engine->GetGameFramework().GetGameState().get();
}
