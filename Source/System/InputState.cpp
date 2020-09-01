/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#include "Precompiled.hpp"
#include "System/InputState.hpp"
using namespace System;

InputState::InputState()
{
    ResetStates();
}

InputState::~InputState() = default;

void System::InputState::UpdateStates(float timeDelta)
{
    // Update input state times.
    for(auto& keyboardKeyState : m_keyboardKeyStates)
    {
        keyboardKeyState.stateTime += timeDelta;
    }

    for(auto& mouseButtonState : m_mouseButtonStates)
    {
        mouseButtonState.stateTime += timeDelta;
    }

    // Transition keyboard key input states.
    for(auto& keyboardKeyState : m_keyboardKeyStates)
    {
        keyboardKeyState.state = TransitionInputState(keyboardKeyState.state);
    }

    for(auto& mouseButtonState : m_mouseButtonStates)
    {
        mouseButtonState.state = TransitionInputState(mouseButtonState.state);
    }

    // Handle case when state time needs resetting after
    // transitioning from PressedReleased to Released state.
    for(auto& keyboardKeyState : m_keyboardKeyStates)
    {
        if(keyboardKeyState.state == InputStates::Released)
        {
            keyboardKeyState.stateTime = 0.0f;
        }
    }

    for(auto& mouseButtonState : m_mouseButtonStates)
    {
        if(mouseButtonState.state == InputStates::Released)
        {
            mouseButtonState.stateTime = 0.0f;
        }
    }
}

void InputState::ResetStates()
{
    // Reset input states.
    for(KeyboardKeys::Type key = KeyboardKeys::Invalid; key < KeyboardKeys::Count; ++key)
    {
        m_keyboardKeyStates[key] = InputEvents::KeyboardKey{ key };
    }

    for(MouseButtons::Type button = MouseButtons::Invalid; button < MouseButtons::Count; ++button)
    {
        m_mouseButtonStates[button] = InputEvents::MouseButton{ button };
    }
}

bool InputState::IsKeyboardKeyPressed(KeyboardKeys::Type key, bool repeat) const
{
    // Validate specified keyboard key.
    if(key <= KeyboardKeys::KeyUnknown || key >= KeyboardKeys::Count)
        return false;

    // Determine if key was pressed.
    return IsInputStatePressed(m_keyboardKeyStates[key].state, repeat);
}

bool InputState::IsKeyboardKeyReleased(KeyboardKeys::Type key, bool repeat) const
{
    // Validate specified keyboard key.
    if(key <= KeyboardKeys::KeyUnknown || key >= KeyboardKeys::Count)
        return false;

    // Determine if key was released.
    return IsInputStateReleased(m_keyboardKeyStates[key].state, repeat);
}

bool InputState::IsMouseButtonPressed(MouseButtons::Type button, bool repeat) const
{
    // Validate specified mouse button.
    if(button <= MouseButtons::Invalid || button >= MouseButtons::Count)
        return false;

    // Determine if button was pressed.
    return IsInputStatePressed(m_mouseButtonStates[button].state, repeat);
}

bool InputState::IsMouseButtonReleased(MouseButtons::Type button, bool repeat) const
{
    // Validate specified mouse button.
    if(button <= MouseButtons::Invalid || button >= MouseButtons::Count)
        return false;

    // Determine if button was released.
    return IsInputStateReleased(m_mouseButtonStates[button].state, repeat);
}

bool InputState::OnTextInput(const InputEvents::TextInput& event)
{
    // Dispatch incoming input event.
    bool inputCaptured = events.textInput.Dispatch(event);
    return inputCaptured;
}

bool InputState::OnKeyboardKey(const InputEvents::KeyboardKey& event)
{
    // Create outgoing keyboard key event.
    InputEvents::KeyboardKey keyboardKeyEvent = m_keyboardKeyStates[event.key];

    if(keyboardKeyEvent.state == InputStates::Pressed && event.state == InputStates::Released)
    {
        // Handle input state changing from pressed to released within a single frame.
        // We do not want to reset state time until we transition to released state.
        keyboardKeyEvent.state = InputStates::PressedReleased;
    }
    else
    {
        keyboardKeyEvent.state = event.state;
        keyboardKeyEvent.stateTime = 0.0f;
    }

    keyboardKeyEvent.modifiers = event.modifiers;

    // Send outgoing keyboard key event.
    bool inputCaptured = events.keyboardKey.Dispatch(keyboardKeyEvent);

    // Save event if not captured or in released state.
    if(!inputCaptured || IsInputStateReleased(keyboardKeyEvent.state))
    {
        m_keyboardKeyStates[event.key] = keyboardKeyEvent;
    }

    // Return whether input event was captured.
    return inputCaptured;
}

bool InputState::OnMouseButton(const InputEvents::MouseButton& event)
{
    // Create outgoing mouse button event.
    InputEvents::MouseButton mouseButtonEvent = m_mouseButtonStates[event.button];

    if(mouseButtonEvent.state == InputStates::Pressed && event.state == InputStates::Released)
    {
        // Handle input state changing from pressed to released within a single frame.
        // We do not want to reset state time until we transition to released state.
        mouseButtonEvent.state = InputStates::PressedReleased;
    }
    else
    {
        mouseButtonEvent.state = event.state;
        mouseButtonEvent.stateTime = 0.0f;
    }

    mouseButtonEvent.modifiers = event.modifiers;

    // Send outgoing mouse button event.
    bool inputCaptured = events.mouseButton.Dispatch(mouseButtonEvent);

    // Save event if not captured or in released state.
    if(!inputCaptured || IsInputStateReleased(mouseButtonEvent.state))
    {
        m_mouseButtonStates[event.button] = mouseButtonEvent;
    }

    // Return whether input event was captured.
    return inputCaptured;
}

bool InputState::OnMouseScroll(const InputEvents::MouseScroll& event)
{
    // Dispatch incoming input event.
    bool inputCaptured = events.mouseScroll.Dispatch(event);
    return inputCaptured;
}

void InputState::OnCursorPosition(const InputEvents::CursorPosition& event)
{
    // Dispatch incoming input event.
    events.cursorPosition.Dispatch(event);
}

void InputState::OnCursorEnter(const InputEvents::CursorEnter& event)
{
    // Dispatch incoming input event.
    events.cursorEnter.Dispatch(event);
}
