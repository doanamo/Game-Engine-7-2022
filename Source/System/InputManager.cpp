/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#include "System/Precompiled.hpp"
#include "System/InputManager.hpp"
#include "System/Window.hpp"
#include <Core/ServiceStorage.hpp>
using namespace System;

InputManager::InputManager() = default;
InputManager::~InputManager()
{
    ASSERT(m_windowContext->inputManager == this);
    m_windowContext->inputManager = nullptr;
}

InputManager::CreateResult InputManager::Create()
{
    LOG("Creating input manager...");
    LOG_SCOPED_INDENT();

    auto instance = std::unique_ptr<InputManager>(new InputManager());
    return Common::Success(std::move(instance));
}

bool InputManager::OnAttach(const Core::ServiceStorage* services)
{
    auto* window = services->Locate<System::Window>();
    if(window == nullptr)
    {
        LOG_ERROR("Failed to locate window service!");
        return false;
    }

    m_windowContext = &window->GetContext();
    if(m_windowContext->inputManager != nullptr)
    {
        LOG_ERROR("Existing input manager already associated with this window context!");
        return false;
    }

    m_windowContext->inputManager = this;

    GLFWwindow* windowHandle = m_windowContext->handle;
    glfwSetKeyCallback(windowHandle, InputManager::KeyboardKeyCallback);
    glfwSetCharCallback(windowHandle, InputManager::TextInputCallback);
    glfwSetMouseButtonCallback(windowHandle, InputManager::MouseButtonCallback);
    glfwSetScrollCallback(windowHandle, InputManager::MouseScrollCallback);
    glfwSetCursorPosCallback(windowHandle, InputManager::CursorPositionCallback);
    glfwSetCursorEnterCallback(windowHandle, InputManager::CursorEnterCallback);

    return true;
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
    InputManager* inputManager = GetInputManagerFromUserData(handle);

    InputEvents::TextInput outgoingEvent;
    outgoingEvent.utf32Character = character;

    inputManager->m_inputState.OnTextInput(outgoingEvent);
}

void InputManager::KeyboardKeyCallback(GLFWwindow* handle,
    int key, int scancode, int action, int mods)
{
    InputManager* inputManager = GetInputManagerFromUserData(handle);

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

    inputManager->m_inputState.OnKeyboardKey(outgoingEvent);
}

void InputManager::MouseButtonCallback(GLFWwindow* handle, int button, int action, int mods)
{
    InputManager* inputManager = GetInputManagerFromUserData(handle);

    InputEvents::MouseButton outgoingEvent;
    outgoingEvent.button = TranslateMouseButton(button);
    outgoingEvent.state = TranslateInputAction(action);
    outgoingEvent.modifiers = TranslateKeyboardModifiers(mods);

    if(outgoingEvent.button <= MouseButtons::Invalid || outgoingEvent.button >= MouseButtons::Count)
    {
        LOG_WARNING("Invalid mouse button input received: {}", button);
        return;
    }

    inputManager->m_inputState.OnMouseButton(outgoingEvent);
}

void InputManager::MouseScrollCallback(GLFWwindow* handle, double offsetx, double offsety)
{
    InputManager* inputManager = GetInputManagerFromUserData(handle);

    InputEvents::MouseScroll outgoingEvent;
    outgoingEvent.offset = offsety;

    inputManager->m_inputState.OnMouseScroll(outgoingEvent);
}

void InputManager::CursorPositionCallback(GLFWwindow* handle, double x, double y)
{
    InputManager* inputManager = GetInputManagerFromUserData(handle);

    InputEvents::CursorPosition outgoingEvent;
    outgoingEvent.x = x;
    outgoingEvent.y = y;

    inputManager->m_inputState.OnCursorPosition(outgoingEvent);
}

void InputManager::CursorEnterCallback(GLFWwindow* handle, int entered)
{
    InputManager* inputManager = GetInputManagerFromUserData(handle);

    InputEvents::CursorEnter outgoingEvent;
    outgoingEvent.entered = entered;

    inputManager->m_inputState.OnCursorEnter(outgoingEvent);
}

