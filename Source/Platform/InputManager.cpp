/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#include "Platform/Precompiled.hpp"
#include "Platform/InputManager.hpp"
#include "Platform/TimerSystem.hpp"
#include "Platform/Timer.hpp"
#include "Platform/WindowSystem.hpp"
#include "Platform/Window.hpp"
#include <Core/SystemStorage.hpp>
using namespace Platform;

namespace
{
    const char* LogAttachFailed = "Failed to attach input manager! {}";
}

InputManager::InputManager()
{
    events.onTickProcessed.Bind([this](float timeDelta)
    {
        UpdateInputState();
    });
}

InputManager::~InputManager()
{
    ASSERT(m_windowContext->inputManager == this);
    m_windowContext->inputManager = nullptr;
}

bool InputManager::OnAttach(const Core::EngineSystemStorage& engineSystems)
{
    // Locate needed engine systems.
    m_timerSystem = &engineSystems.Locate<Platform::TimerSystem>();

    // Register input manager with current window context.
    auto& windowSystem = engineSystems.Locate<Platform::WindowSystem>();
    m_windowContext = &windowSystem.GetWindow().GetContext();
    if(m_windowContext->inputManager != nullptr)
    {
        LOG_ERROR("Existing input manager already associated with this window context!");
        return false;
    }

    m_windowContext->inputManager = this;

    // Set window input callbacks.
    GLFWwindow* windowHandle = m_windowContext->handle;
    glfwSetKeyCallback(windowHandle, InputManager::KeyboardKeyCallback);
    glfwSetCharCallback(windowHandle, InputManager::TextInputCallback);
    glfwSetMouseButtonCallback(windowHandle, InputManager::MouseButtonCallback);
    glfwSetScrollCallback(windowHandle, InputManager::MouseScrollCallback);
    glfwSetCursorPosCallback(windowHandle, InputManager::CursorPositionCallback);
    glfwSetCursorEnterCallback(windowHandle, InputManager::CursorEnterCallback);

    return true;
}

void InputManager::UpdateInputState()
{
    m_inputState.UpdateStates(m_timerSystem->GetTimer().GetDeltaSeconds());
}

void InputManager::ResetInputState()
{
    m_inputState.ResetStates();
}

InputManager* InputManager::GetInputManagerFromUserData(GLFWwindow* handle)
{
    ASSERT(handle != nullptr, "Window handle is invalid!");
    WindowContext* context = reinterpret_cast<WindowContext*>(glfwGetWindowUserPointer(handle));
    ASSERT(context != nullptr, "Window context is null!");

    InputManager* inputManager = context->inputManager;
    ASSERT(inputManager != nullptr, "Input manager in window context is null!");
    return inputManager;
}

void InputManager::TextInputCallback(GLFWwindow* handle, unsigned int character)
{
    InputEvents::TextInput outgoingEvent;
    outgoingEvent.utf32Character = character;

    InputManager* inputManager = GetInputManagerFromUserData(handle);
    inputManager->m_inputState.OnTextInput(outgoingEvent);
}

void InputManager::KeyboardKeyCallback(GLFWwindow* handle,
    int key, int scancode, int action, int mods)
{
    InputEvents::KeyboardKey outgoingEvent;
    outgoingEvent.key = TranslateKeyboardKey(key);
    outgoingEvent.state = TranslateInputAction(action);
    outgoingEvent.modifiers = TranslateKeyboardModifiers(mods);

    if(outgoingEvent.key <= KeyboardKeys::Invalid || outgoingEvent.key >= KeyboardKeys::Count)
    {
        LOG_WARNING("Invalid keyboard key input received: {}", key);
        return;
    }

    if(outgoingEvent.key == KeyboardKeys::KeyUnknown)
    {
        LOG_WARNING("Unknown keyboard key input received: {}", key);
        return;
    }

    InputManager* inputManager = GetInputManagerFromUserData(handle);
    inputManager->m_inputState.OnKeyboardKey(outgoingEvent);
}

void InputManager::MouseButtonCallback(GLFWwindow* handle, int button, int action, int mods)
{
    InputEvents::MouseButton outgoingEvent;
    outgoingEvent.button = TranslateMouseButton(button);
    outgoingEvent.state = TranslateInputAction(action);
    outgoingEvent.modifiers = TranslateKeyboardModifiers(mods);

    if(outgoingEvent.button <= MouseButtons::Invalid || outgoingEvent.button >= MouseButtons::Count)
    {
        LOG_WARNING("Invalid mouse button input received: {}", button);
        return;
    }

    InputManager* inputManager = GetInputManagerFromUserData(handle);
    inputManager->m_inputState.OnMouseButton(outgoingEvent);
}

void InputManager::MouseScrollCallback(GLFWwindow* handle, double offsetx, double offsety)
{
    InputEvents::MouseScroll outgoingEvent;
    outgoingEvent.offset = offsety;

    InputManager* inputManager = GetInputManagerFromUserData(handle);
    inputManager->m_inputState.OnMouseScroll(outgoingEvent);
}

void InputManager::CursorPositionCallback(GLFWwindow* handle, double x, double y)
{
    InputEvents::CursorPosition outgoingEvent;
    outgoingEvent.x = x;
    outgoingEvent.y = y;

    InputManager* inputManager = GetInputManagerFromUserData(handle);
    inputManager->m_inputState.OnCursorPosition(outgoingEvent);
}

void InputManager::CursorEnterCallback(GLFWwindow* handle, int entered)
{
    InputEvents::CursorEnter outgoingEvent;
    outgoingEvent.entered = entered;

    InputManager* inputManager = GetInputManagerFromUserData(handle);
    inputManager->m_inputState.OnCursorEnter(outgoingEvent);
}
