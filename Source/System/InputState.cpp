/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#include "System/Precompiled.hpp"
#include "System/InputState.hpp"
using namespace System;

InputState::InputState()
{
    ResetStates();

    events.Register<bool, InputEvents::TextInput>(std::make_unique<Event::CollectWhileFalse>());
    events.Register<bool, InputEvents::KeyboardKey>(std::make_unique<Event::CollectWhileFalse>());
    events.Register<bool, InputEvents::MouseButton>(std::make_unique<Event::CollectWhileFalse>());
    events.Register<bool, InputEvents::MouseScroll>(std::make_unique<Event::CollectWhileFalse>());
    events.Register<void, InputEvents::CursorPosition>();
    events.Register<void, InputEvents::CursorEnter>();
    events.Finalize();
}

InputState::~InputState() = default;

void InputState::UpdateStates(float timeDelta)
{
    UpdateStateTimes(timeDelta);
    UpdateStateTransitions();
}

void InputState::UpdateStateTimes(float timeDelta)
{
    for(auto& keyboardKeyState : m_keyboardKeyStates)
    {
        keyboardKeyState.stateTime += timeDelta;
    }

    for(auto& mouseButtonState : m_mouseButtonStates)
    {
        mouseButtonState.stateTime += timeDelta;
    }
}

void InputState::UpdateStateTransitions()
{
    for(auto& keyboardKeyState : m_keyboardKeyStates)
    {
        keyboardKeyState.state = TransitionInputState(keyboardKeyState.state);

        if(keyboardKeyState.state == InputStates::Released)
        {
            keyboardKeyState.stateTime = 0.0f;
        }
    }

    for(auto& mouseButtonState : m_mouseButtonStates)
    {
        mouseButtonState.state = TransitionInputState(mouseButtonState.state);

        if(mouseButtonState.state == InputStates::Released)
        {
            mouseButtonState.stateTime = 0.0f;
        }
    }
}

void InputState::ResetStates()
{
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
    if(key <= KeyboardKeys::KeyUnknown || key >= KeyboardKeys::Count)
        return false;

    return IsInputStatePressed(m_keyboardKeyStates[key].state, repeat);
}

bool InputState::IsKeyboardKeyReleased(KeyboardKeys::Type key, bool repeat) const
{
    if(key <= KeyboardKeys::KeyUnknown || key >= KeyboardKeys::Count)
        return false;

    return IsInputStateReleased(m_keyboardKeyStates[key].state, repeat);
}

bool InputState::IsMouseButtonPressed(MouseButtons::Type button, bool repeat) const
{
    if(button <= MouseButtons::Invalid || button >= MouseButtons::Count)
        return false;

    return IsInputStatePressed(m_mouseButtonStates[button].state, repeat);
}

bool InputState::IsMouseButtonReleased(MouseButtons::Type button, bool repeat) const
{
    if(button <= MouseButtons::Invalid || button >= MouseButtons::Count)
        return false;

    return IsInputStateReleased(m_mouseButtonStates[button].state, repeat);
}

bool InputState::OnTextInput(const InputEvents::TextInput& event)
{
    bool inputCaptured = events.Dispatch<bool>(event).Unwrap();
    return inputCaptured;
}

bool InputState::OnKeyboardKey(const InputEvents::KeyboardKey& event)
{
    InputEvents::KeyboardKey keyboardKeyEvent = m_keyboardKeyStates[event.key];

    // Check whether we are still in intermediate state triggered during same frame.
    if(keyboardKeyEvent.state == InputStates::Pressed && event.state == InputStates::Released)
    {
        // State changes from pressed to released in single frame.
        keyboardKeyEvent.state = InputStates::PressedReleased;
    }
    else
    {
        // Change to new state.
        keyboardKeyEvent.state = event.state;
        keyboardKeyEvent.modifiers = event.modifiers;
        keyboardKeyEvent.stateTime = 0.0f;
    }

    // Dispatch input event and determine whether input has been captured.
    bool inputCaptured = events.Dispatch<bool>(keyboardKeyEvent).Unwrap();
    if(!inputCaptured || IsInputStateReleased(keyboardKeyEvent.state))
    {
        m_keyboardKeyStates[event.key] = keyboardKeyEvent;
    }

    return inputCaptured;
}

bool InputState::OnMouseButton(const InputEvents::MouseButton& event)
{
    InputEvents::MouseButton mouseButtonEvent = m_mouseButtonStates[event.button];

    // Check whether we are still in intermediate state triggered during same frame.
    if(mouseButtonEvent.state == InputStates::Pressed && event.state == InputStates::Released)
    {
        // State changes from pressed to released in single frame.
        mouseButtonEvent.state = InputStates::PressedReleased;
    }
    else
    {
        // Change to new state.
        mouseButtonEvent.state = event.state;
        mouseButtonEvent.modifiers = event.modifiers;
        mouseButtonEvent.stateTime = 0.0f;
    }

    // Dispatch input event and determine whether input has been captured.
    bool inputCaptured = events.Dispatch<bool>(mouseButtonEvent).Unwrap();
    if(!inputCaptured || IsInputStateReleased(mouseButtonEvent.state))
    {
        m_mouseButtonStates[event.button] = mouseButtonEvent;
    }

    return inputCaptured;
}

bool InputState::OnMouseScroll(const InputEvents::MouseScroll& event)
{
    bool inputCaptured = events.Dispatch<bool>(event).Unwrap();
    return inputCaptured;
}

void InputState::OnCursorPosition(const InputEvents::CursorPosition& event)
{
    events.Dispatch<void>(event);
}

void InputState::OnCursorEnter(const InputEvents::CursorEnter& event)
{
    events.Dispatch<void>(event);
}
