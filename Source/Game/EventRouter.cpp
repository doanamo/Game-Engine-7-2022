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

EventRouter::CreateResult EventRouter::Create(const CreateFromParams& params)
{
    LOG("Creating event router...");
    LOG_SCOPED_INDENT();

    // Validate arguments.
    CHECK_ARGUMENT_OR_RETURN(params.inputManager != nullptr, Common::Failure(CreateErrors::InvalidArgument));
    CHECK_ARGUMENT_OR_RETURN(params.gameFramework != nullptr, Common::Failure(CreateErrors::InvalidArgument));

    // Create instance.
    auto instance = std::unique_ptr<EventRouter>(new EventRouter());

    // Subscribe event receivers.
    bool subscriptionResult = true;
    subscriptionResult &= instance->m_receivers.keyboardKeyReceiver.Subscribe(params.inputManager->events.keyboardKey);
    subscriptionResult &= instance->m_receivers.textInputReceiver.Subscribe(params.inputManager->events.textInput);
    subscriptionResult &= instance->m_receivers.mouseButtonReceiver.Subscribe(params.inputManager->events.mouseButton);
    subscriptionResult &= instance->m_receivers.mouseScrollReceiver.Subscribe(params.inputManager->events.mouseScroll);
    subscriptionResult &= instance->m_receivers.cursorPosition.Subscribe(params.inputManager->events.cursorPosition);
    subscriptionResult &= instance->m_receivers.cursorEnter.Subscribe(params.inputManager->events.cursorEnter);

    if(!subscriptionResult)
    {
        LOG_ERROR("Could not subscribe event receivers!");
        return Common::Failure(CreateErrors::FailedEventSubscription);
    }

    // Save game framework reference.
    instance->m_gameFramework = params.gameFramework;

    // Success!
    return Common::Success(std::move(instance));
}

GameState* EventRouter::GetCurrentGameState()
{
    return m_gameFramework->GetGameState().get();
}
