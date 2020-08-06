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

InputState::CreateResult System::InputState::Create()
{
    LOG("Creating input state...");
    LOG_SCOPED_INDENT();

    // Create instance.
    auto instance = std::unique_ptr<InputState>(new InputState());

    // Success!
    return Common::Success(std::move(instance));
}

void System::InputState::UpdateStates(float timeDelta)
{
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

void InputState::ResetStates()
{
    // Reset keyboard key states.
    for(KeyboardKeys::Type key = KeyboardKeys::Invalid; key < KeyboardKeys::Count; ++key)
    {
        m_keyboardKeyStates[key] = InputEvents::KeyboardKey{ key };
    }
}

bool InputState::IsKeyboardKeyPressed(KeyboardKeys::Type key, bool repeat)
{
    // Validate specified keyboard key.
    if(key <= KeyboardKeys::KeyUnknown || key >= KeyboardKeys::Count)
        return false;

    // Determine if key was pressed.
    return m_keyboardKeyStates[key].IsPressed(repeat);
}

bool InputState::IsKeyboardKeyReleased(KeyboardKeys::Type key, bool repeat)
{
    // Validate specified keyboard key.
    if(key <= KeyboardKeys::KeyUnknown || key >= KeyboardKeys::Count)
        return false;

    // Determine if key was released.
    return m_keyboardKeyStates[key].IsReleased(repeat);
}

bool InputState::OnKeyboardKey(const InputEvents::KeyboardKey& event)
{
    // Send outgoing keyboard key event.
    InputEvents::KeyboardKey keyboardKeyEvent = m_keyboardKeyStates[event.key];

    if(keyboardKeyEvent.state == InputStates::Pressed && event.state == InputStates::Released)
    {
        // Handle keyboard keys being pressed and released quickly within a single frame.
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

    // Save new keyboard key event in cases when it
    // is not captured or when it is in released state.
    if(!inputCaptured || keyboardKeyEvent.IsReleased())
    {
        m_keyboardKeyStates[event.key] = keyboardKeyEvent;
    }

    // Return whether input event was captured.
    return inputCaptured;
}

bool InputState::OnTextInput(const InputEvents::TextInput& event)
{
    // Dispatch incoming input event.
    bool inputCaptured = events.textInput.Dispatch(event);
    return inputCaptured;
}

bool InputState::OnMouseButton(const InputEvents::MouseButton& event)
{
    // Dispatch incoming input event.
    bool inputCaptured = events.mouseButton.Dispatch(event);
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
