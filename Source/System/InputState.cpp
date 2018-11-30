/*
    Copyright (c) 2018 Piotr Doan. All rights reserved.
*/

#include "Precompiled.hpp"
#include "System/InputState.hpp"
using namespace System;

InputState::InputState() :
    m_initialized(false)
{
    // Setup initial input state.
    this->Reset();

    // Bind event receivers.
    m_keyboardKey.Bind<InputState, &InputState::OnKeyboardKey>(this);
    m_windowFocus.Bind<InputState, &InputState::OnWindowFocus>(this);
}

InputState::~InputState()
{
}

InputState::InputState(InputState&& other) :
    InputState()
{
    // Call move operator.
    *this = std::move(other);
}

InputState& InputState::operator=(InputState&& other)
{
    // Swap class members.
    std::swap(m_initialized, other.m_initialized);
    std::swap(m_keyboardState, other.m_keyboardState);
    std::swap(m_keyboardKey, other.m_keyboardKey);
    std::swap(m_windowFocus, other.m_windowFocus);

    return *this;
}

void InputState::Reset()
{
    // Set all keys to their untouched state.
    for(auto& i : m_keyboardState)
    {
        i = KeyboardKeyStates::ReleasedRepeat;
    }
}

bool InputState::Initialize(Window* window)
{
    LOG() << "Initializing input state..." << LOG_INDENT();

    // Validate arguments.
    if(window == nullptr)
    {
        LOG_ERROR() << "Invalid argument - \"window\" is null!";
        return false;
    }

    // Subscribe to window's event receivers.
    bool subscribedSuccessfully = true;

    subscribedSuccessfully &= m_keyboardKey.Subscribe(window->events.keyboardKey);
    subscribedSuccessfully &= m_windowFocus.Subscribe(window->events.focus);

    if(!subscribedSuccessfully)
    {
        LOG_ERROR() << "Could not subscribe to window event receivers!";
        return false;
    }

    // Success!
    return m_initialized = true;
}

bool InputState::OnKeyboardKey(const Window::Events::KeyboardKey& event)
{
    ASSERT(m_initialized, "Input system has not been initialized!");
    VERIFY(0 <= event.key && event.key < KeyboardKeys::Count, "Received an event with an invalid key!");

    // Handle keyboard input events.
    if(event.state == InputStates::Pressed)
    {
        m_keyboardState[event.key] = KeyboardKeyStates::Pressed;
    }
    else if(event.state == InputStates::Released)
    {
        m_keyboardState[event.key] = KeyboardKeyStates::Released;
    }

    // Do not capture input.
    return false;
}

void InputState::OnWindowFocus(const Window::Events::Focus& event)
{
    ASSERT(m_initialized, "Input system has not been initialized!");

    // Reset key states on window focus loss.
    // This will help avoid stuck input states after switching between applications.
    if(!event.focused)
    {
        this->Reset();
    }
}

void InputState::PrepareForEvents()
{
    ASSERT(m_initialized, "Input system has not been initialized!");

    // Update repeating key states.
    for(auto& state : m_keyboardState)
    {
        if(state == KeyboardKeyStates::Pressed)
        {
            state = KeyboardKeyStates::PressedRepeat;
        }
        else if(state == KeyboardKeyStates::Released)
        {
            state = KeyboardKeyStates::ReleasedRepeat;
        }
    }
}

bool InputState::IsKeyboardKeyDown(int key, bool repeat)
{
    ASSERT(m_initialized, "Input system has not been initialized!");
    VERIFY(0 <= key && key < KeyboardKeys::Count, "Attempting to index an invalid key!");

    // Check if the key was just pressed.
    if(m_keyboardState[key] == KeyboardKeyStates::Pressed)
        return true;

    // Check if the key is in repeating pressed state.
    if(repeat && m_keyboardState[key] == KeyboardKeyStates::PressedRepeat)
        return true;

    return false;
}

bool InputState::IsKeyboardKeyUp(int key, bool repeat)
{
    ASSERT(m_initialized, "Input system has not been initialized!");
    VERIFY(0 <= key && key < KeyboardKeys::Count, "Attempting to index an invalid key!");

    // Check if the key was just released.
    if(m_keyboardState[key] == KeyboardKeyStates::Released)
        return true;

    // Check if the key is in repeating released state.
    if(repeat && m_keyboardState[key] == KeyboardKeyStates::ReleasedRepeat)
        return true;

    return false;
}
