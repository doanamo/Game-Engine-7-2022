/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#include "Precompiled.hpp"
#include "System/InputManager.hpp"
#include "System/Window.hpp"
using namespace System;

InputManager::InputManager() :
    m_initialized(false)
{
    // Bind event receivers.
    m_receivers.keyboardKey.Bind<InputManager, &InputManager::OnKeyboardKey>(this);
    m_receivers.textInput.Bind<InputManager, &InputManager::OnTextInput>(this);
    m_receivers.mouseButton.Bind<InputManager, &InputManager::OnMouseButton>(this);
    m_receivers.mouseScroll.Bind<InputManager, &InputManager::OnMouseScroll>(this);
    m_receivers.cursorPosition.Bind<InputManager, &InputManager::OnCursorPosition>(this);
    m_receivers.cursorEnter.Bind<InputManager, &InputManager::OnCursorEnter>(this);
}

InputManager::~InputManager()
{
}

InputManager::InputManager(InputManager&& other) :
    InputManager()
{
    *this = std::move(other);
}

InputManager& InputManager::operator=(InputManager&& other)
{
    std::swap(events, other.events);
    std::swap(m_receivers, other.m_receivers);
    std::swap(m_keyboardKeyStates, other.m_keyboardKeyStates);
    std::swap(m_initialized, other.m_initialized);

    return *this;
}

bool InputManager::Initialize(Window* window)
{
    LOG() << "Initializing input manager..." << LOG_INDENT();

    // Validate arguments.
    if(window == nullptr && !window->IsValid())
    {
        LOG_ERROR() << "Invalid argument - \"window\" is invalid!";
        return false;
    }

    // Subscribe to window input events.
    bool subscriptionResult = true;

    subscriptionResult &= m_receivers.keyboardKey.Subscribe(window->events.keyboardKey);
    subscriptionResult &= m_receivers.textInput.Subscribe(window->events.textInput);
    subscriptionResult &= m_receivers.mouseButton.Subscribe(window->events.mouseButton);
    subscriptionResult &= m_receivers.mouseScroll.Subscribe(window->events.mouseScroll);
    subscriptionResult &= m_receivers.cursorPosition.Subscribe(window->events.cursorPosition);
    subscriptionResult &= m_receivers.cursorEnter.Subscribe(window->events.cursorEnter);

    if(!subscriptionResult)
    {
        LOG_ERROR() << "Could not subscribe to window input events!";
        return false;
    }

    // Reset input states.
    this->ResetStates();

    // Success!
    return m_initialized = true;
}

void InputManager::AdvanceState(float timeDelta)
{
    ASSERT(m_initialized, "Input system has not been initialized!");

    // Update state times for all keyboard keys.
    for(auto& keyboardKeyState : m_keyboardKeyStates)
    {
        keyboardKeyState.stateTime += timeDelta;
    }

    // Transition keyboard key input states.
    for(auto& keyboardKeyState : m_keyboardKeyStates)
    {
        switch(keyboardKeyState.state)
        {
        case InputStates::Pressed:
            keyboardKeyState.state = InputStates::PressedRepeat;
            break;

        case InputStates::PressedReleased:
            keyboardKeyState.state = InputStates::Released;
            keyboardKeyState.stateTime = 0.0f;
            break;

        case InputStates::Released:
            keyboardKeyState.state = InputStates::ReleasedRepeat;
            break;
        }
    }
}

void InputManager::ResetStates()
{
    // Reset keyboard key states.
    for(KeyboardKeys::Type key = KeyboardKeys::Invalid; key < KeyboardKeys::Count; ++key)
    {
        m_keyboardKeyStates[key] = InputEvents::KeyboardKey(key);
    }
}

bool InputManager::IsKeyboardKeyPressed(KeyboardKeys::Type key, bool repeat)
{
    ASSERT(m_initialized, "Input system has not been initialized!");

    // Validate specified keyboard key.
    if(key <= KeyboardKeys::KeyUnknown || key >= KeyboardKeys::Count)
        return false;

    // Determine if key was pressed.
    return m_keyboardKeyStates[key].IsPressed(repeat);
}

bool InputManager::IsKeyboardKeyReleased(KeyboardKeys::Type key, bool repeat)
{
    ASSERT(m_initialized, "Input system has not been initialized!");

    // Validate specified keyboard key.
    if(key <= KeyboardKeys::KeyUnknown || key >= KeyboardKeys::Count)
        return false;

    // Determine if key was released.
    return m_keyboardKeyStates[key].IsReleased(repeat);
}

bool InputManager::OnKeyboardKey(const Window::Events::KeyboardKey& event)
{
    ASSERT(m_initialized, "Input system has not been initialized!");

    // Translate keyboard event.
    KeyboardKeys::Type key = TranslateKeyboardKey(event.key);
    InputStates::Type state = TranslateInputAction(event.action);
    KeyboardModifiers::Type modifiers = TranslateKeyboardModifiers(event.modifiers);

    // Validate key index.
    if(key <= KeyboardKeys::Invalid || key >= KeyboardKeys::Count)
    {
        LOG_WARNING() << "Invalid keyboard key input received: " << event.key;
        return false;
    }

    if(key == KeyboardKeys::KeyUnknown)
    {
        LOG_WARNING() << "Unknown keyboard key input received: " << event.key;
        return false;
    }

    // Send an outgoing keyboard key event.
    InputEvents::KeyboardKey keyboardKeyEvent = m_keyboardKeyStates[key];

    if(keyboardKeyEvent.state == InputStates::Pressed && state == InputStates::Released)
    {
        // Handle keyboard keys being pressed and released quickly within a single frame.
        // We do not want to reset state time until we transition to released state.
        keyboardKeyEvent.state = InputStates::PressedReleased;
    }
    else
    {
        keyboardKeyEvent.state = state;
        keyboardKeyEvent.stateTime = 0.0f;
    }

    keyboardKeyEvent.modifiers = modifiers;
 
    // Send an outgoing keyboard key event.
    bool inputCaptured = events.keyboardKey.Dispatch(keyboardKeyEvent);

    // Save new keyboard key event in cases when it
    // is not captured or when it is in released state.
    if(!inputCaptured || keyboardKeyEvent.IsReleased())
    {
        m_keyboardKeyStates[key] = keyboardKeyEvent;
    }

    // Do not consume input event.
    return false;
}

bool InputManager::OnTextInput(const Window::Events::TextInput& event)
{
    ASSERT(m_initialized, "Input system has not been initialized!");

    // Send an outgoing text input event.
    InputEvents::TextInput outgoingEvent;
    outgoingEvent.utf32Character = event.utf32Character;

    events.textInput.Dispatch(outgoingEvent);

    // Do not consume input event.
    return false;
}

bool InputManager::OnMouseButton(const Window::Events::MouseButton& event)
{
    ASSERT(m_initialized, "Input system has not been initialized!");

    // Send an outgoing mouse button event.
    InputEvents::MouseButton outgoingEvent;
    outgoingEvent.button = TranslateMouseButton(event.button);
    outgoingEvent.state = TranslateInputAction(event.action);
    outgoingEvent.modifiers = TranslateKeyboardModifiers(event.modifiers);

    events.mouseButton.Dispatch(outgoingEvent);

    // Do not consume input event.
    return false;
}

bool InputManager::OnMouseScroll(const Window::Events::MouseScroll& event)
{
    ASSERT(m_initialized, "Input system has not been initialized!");

    // Send an outgoing mouse scroll event.
    InputEvents::MouseScroll outgoingEvent;
    outgoingEvent.offset = event.offset;

    events.mouseScroll.Dispatch(outgoingEvent);

    // Do not consume input event.
    return false;
}

void InputManager::OnCursorPosition(const Window::Events::CursorPosition& event)
{
    ASSERT(m_initialized, "Input system has not been initialized!");

    // Send an outgoing cursor position event.
    InputEvents::CursorPosition outgoingEvent;
    outgoingEvent.x = event.x;
    outgoingEvent.y = event.y;

    events.cursorPosition.Dispatch(outgoingEvent);
}

void InputManager::OnCursorEnter(const Window::Events::CursorEnter& event)
{
    ASSERT(m_initialized, "Input system has not been initialized!");

    // Send an outgoing cursor enter event.
    InputEvents::CursorEnter outgoingEvent;
    outgoingEvent.entered = event.entered;

    events.cursorEnter.Dispatch(outgoingEvent);
}
