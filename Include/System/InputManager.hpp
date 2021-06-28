/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#pragma once

#include <Core/EngineSystem.hpp>
#include "System/InputState.hpp"
#include "System/WindowEvents.hpp"

/*
    Input Manager

    Listens to all input related events from window and propagates them to current input state.
*/

namespace System
{
    class WindowContext;

    class InputManager final : public Core::EngineSystem
    {
        REFLECTION_ENABLE(InputManager, Core::EngineSystem)

    public:
        InputManager();
        ~InputManager() override;

        void UpdateInputState(float timeDelta);
        void ResetInputState();

        InputState& GetInputState();

    private:
        bool OnAttach(const Core::EngineSystemStorage& engineSystems) override;

        static InputManager* GetInputManagerFromUserData(GLFWwindow* handle);
        static void TextInputCallback(GLFWwindow* handle, unsigned int character);
        static void KeyboardKeyCallback(GLFWwindow* handle, int key, int scancode, int action, int mods);
        static void MouseButtonCallback(GLFWwindow* handle, int button, int action, int mods);
        static void MouseScrollCallback(GLFWwindow* handle, double offsetx, double offsety);
        static void CursorPositionCallback(GLFWwindow* handle, double x, double y);
        static void CursorEnterCallback(GLFWwindow* handle, int entered);

    private:
        WindowContext* m_windowContext = nullptr;
        InputState m_inputState;
    };
}

REFLECTION_TYPE(System::InputManager, Core::EngineSystem)
