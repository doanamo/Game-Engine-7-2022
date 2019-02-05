/*
    Copyright (c) 2018-2019 Piotr Doan. All rights reserved.
*/

#include "Precompiled.hpp"
#include "System/InputManager.hpp"
using namespace System;

InputManager::InputManager() :
    m_initialized(false)
{
    // Bind event receivers.
    m_windowFocus.Bind<InputManager, &InputManager::OnWindowFocus>(this);
    m_keyboardKey.Bind<InputManager, &InputManager::OnKeyboardKey>(this);
}

InputManager::~InputManager()
{
}

InputManager::InputManager(InputManager&& other) :
    InputManager()
{
    // Call move operator.
    *this = std::move(other);
}

InputManager& InputManager::operator=(InputManager&& other)
{
    // Swap class members.
    std::swap(m_windowFocus, other.m_windowFocus);
    std::swap(m_keyboardKey, other.m_keyboardKey);
    std::swap(m_initialized, other.m_initialized);

    return *this;
}

bool InputManager::Initialize(Window* window)
{
    LOG() << "Initializing input manager..." << LOG_INDENT();

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

bool InputManager::OnKeyboardKey(const Window::Events::KeyboardKey& event)
{
    ASSERT(m_initialized, "Input system has not been initialized!");
    VERIFY(0 <= event.key && event.key < KeyboardKeys::Count, "Received an event with an invalid key!");

    // Route input event to game state.
    // #todo: Use EventRouter class to do this.

    // Do not capture input.
    return false;
}

void InputManager::OnWindowFocus(const Window::Events::Focus& event)
{
    ASSERT(m_initialized, "Input system has not been initialized!");

    // Request input state to be reset via an event.
    // This will help avoid stuck input states after losing focus.
    if(!event.focused)
    {
        // #todo: Use EventRouter class to do this. 
    }
}

void InputManager::PrepareForEvents()
{
    ASSERT(m_initialized, "Input system has not been initialized!");

    // #todo: Determine if this will be needed.
}
