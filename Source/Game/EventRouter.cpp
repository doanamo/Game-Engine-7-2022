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
    m_receivers.keyboardKeyReceiver.Bind<EventRouter, &EventRouter::PushEventReturnFalse<System::InputEvents::KeyboardKey>>(this);
    m_receivers.textInputReceiver.Bind<EventRouter, &EventRouter::PushEventReturnFalse<System::InputEvents::TextInput>>(this);
    m_receivers.mouseButtonReceiver.Bind<EventRouter, &EventRouter::PushEventReturnFalse<System::InputEvents::MouseButton>>(this);
    m_receivers.mouseScrollReceiver.Bind<EventRouter, &EventRouter::PushEventReturnFalse<System::InputEvents::MouseScroll>>(this);
    m_receivers.cursorPosition.Bind<EventRouter, &EventRouter::PushEventReturnVoid<System::InputEvents::CursorPosition>>(this);
    m_receivers.cursorEnter.Bind<EventRouter, &EventRouter::PushEventReturnVoid<System::InputEvents::CursorEnter>>(this);
}

EventRouter::~EventRouter() = default;

EventRouter::InitializeResult EventRouter::Initialize(const InitializeFromParams& params)
{
    LOG("Initializing event router...");
    LOG_SCOPED_INDENT();

    // Setup initialization guard.
    VERIFY(!m_initialized, "Instance has already been initialized!");
    SCOPE_GUARD_IF(!m_initialized, this->Reset());

    // Validate arguments.
    CHECK_ARGUMENT_OR_RETURN(params.inputManager != nullptr, Failure(InitializeErrors::InvalidArgument));
    CHECK_ARGUMENT_OR_RETURN(params.gameFramework != nullptr, Failure(InitializeErrors::InvalidArgument));

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
        return Failure(InitializeErrors::FailedEventSubscription);
    }

    // Success!
    m_initialized = true;
    return Success();
}

GameState* EventRouter::GetCurrentGameState()
{
    ASSERT(m_initialized, "Event listener has not been initialized!");
    return m_gameFramework->GetGameState().get();
}
