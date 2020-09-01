/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#include "Precompiled.hpp"
#include "System/InputManager.hpp"
#include "System/Window.hpp"
using namespace System;

InputManager::InputManager()
{
    m_receivers.keyboardKey.Bind<InputManager, &InputManager::OnKeyboardKey>(this);
    m_receivers.textInput.Bind<InputManager, &InputManager::OnTextInput>(this);
    m_receivers.mouseButton.Bind<InputManager, &InputManager::OnMouseButton>(this);
    m_receivers.mouseScroll.Bind<InputManager, &InputManager::OnMouseScroll>(this);
    m_receivers.cursorPosition.Bind<InputManager, &InputManager::OnCursorPosition>(this);
    m_receivers.cursorEnter.Bind<InputManager, &InputManager::OnCursorEnter>(this);
}

InputManager::~InputManager() = default;

InputManager::CreateResult InputManager::Create(const CreateParams& params)
{
    LOG("Creating input manager...");
    LOG_SCOPED_INDENT();

    // Validate arguments.
    CHECK_ARGUMENT_OR_RETURN(params.services != nullptr, Common::Failure(CreateErrors::InvalidArgument));

    // Acquire window service.
    Window* window = params.services->GetWindow();

    // Create instance.
    auto instance = std::unique_ptr<InputManager>(new InputManager());

    // Subscribe to window input events.
    bool subscriptionResult = true;
    subscriptionResult &= instance->m_receivers.keyboardKey.Subscribe(window->events.keyboardKey);
    subscriptionResult &= instance->m_receivers.textInput.Subscribe(window->events.textInput);
    subscriptionResult &= instance->m_receivers.mouseButton.Subscribe(window->events.mouseButton);
    subscriptionResult &= instance->m_receivers.mouseScroll.Subscribe(window->events.mouseScroll);
    subscriptionResult &= instance->m_receivers.cursorPosition.Subscribe(window->events.cursorPosition);
    subscriptionResult &= instance->m_receivers.cursorEnter.Subscribe(window->events.cursorEnter);

    if(!subscriptionResult)
    {
        LOG_ERROR("Could not subscribe to window input events!");
        return Common::Failure(CreateErrors::FailedEventSubscription);
    }

    // Success!
    return Common::Success(std::move(instance));
}

void InputManager::UpdateInputState(float timeDelta)
{
    m_inputState.UpdateStates(timeDelta);
}

void InputManager::ResetInputState()
{
    m_inputState.ResetStates();
}

InputState& InputManager::GetInputState()
{
    return m_inputState;
}

bool InputManager::OnTextInput(const Window::Events::TextInput& event)
{
    // Translate incoming window event.
    InputEvents::TextInput outgoingEvent;
    outgoingEvent.utf32Character = event.utf32Character;

    // Propagate event to input state.
    m_inputState.OnTextInput(outgoingEvent);

    // Do not consume window event.
    return false;
}

bool InputManager::OnKeyboardKey(const Window::Events::KeyboardKey& event)
{
    // Translate incoming window event.
    InputEvents::KeyboardKey outgoingEvent;
    outgoingEvent.key = TranslateKeyboardKey(event.key);
    outgoingEvent.state = TranslateInputAction(event.action);
    outgoingEvent.modifiers = TranslateKeyboardModifiers(event.modifiers);

    // Validate keyboard key index.
    if(outgoingEvent.key <= KeyboardKeys::Invalid || outgoingEvent.key >= KeyboardKeys::Count)
    {
        LOG_WARNING("Invalid keyboard key input received: {}", event.key);
        return false;
    }

    if(outgoingEvent.key == KeyboardKeys::KeyUnknown)
    {
        LOG_WARNING("Unknown keyboard key input received: {}", event.key);
        return false;
    }

    // Propagate event to input state.
    // We do not take capturing into consideration in case of release key events.
    m_inputState.OnKeyboardKey(outgoingEvent);

    // Do not consume window event.
    return false;
}

bool InputManager::OnMouseButton(const Window::Events::MouseButton& event)
{
    // Translate incoming window event.
    InputEvents::MouseButton outgoingEvent;
    outgoingEvent.button = TranslateMouseButton(event.button);
    outgoingEvent.state = TranslateInputAction(event.action);
    outgoingEvent.modifiers = TranslateKeyboardModifiers(event.modifiers);

    // Validate mouse button index.
    if(outgoingEvent.button <= MouseButtons::Invalid || outgoingEvent.button >= MouseButtons::Count)
    {
        LOG_WARNING("Invalid mouse button input received: {}", event.button);
        return false;
    }

    // Propagate event to input state.
    m_inputState.OnMouseButton(outgoingEvent);

    // Do not consume window event.
    return false;
}

bool InputManager::OnMouseScroll(const Window::Events::MouseScroll& event)
{
    // Translate incoming window event.
    InputEvents::MouseScroll outgoingEvent;
    outgoingEvent.offset = event.offset;

    // Propagate event to input state.
    m_inputState.OnMouseScroll(outgoingEvent);

    // Do not consume window event.
    return false;
}

void InputManager::OnCursorPosition(const Window::Events::CursorPosition& event)
{
    // Translate incoming window event.
    InputEvents::CursorPosition outgoingEvent;
    outgoingEvent.x = event.x;
    outgoingEvent.y = event.y;

    // Propagate event to input state.
    m_inputState.OnCursorPosition(outgoingEvent);
}

void InputManager::OnCursorEnter(const Window::Events::CursorEnter& event)
{
    // Translate incoming window event.
    InputEvents::CursorEnter outgoingEvent;
    outgoingEvent.entered = event.entered;

    // Propagate event to input state.
    m_inputState.OnCursorEnter(outgoingEvent);
}
