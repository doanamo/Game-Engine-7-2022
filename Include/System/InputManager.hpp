/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#pragma once

#include <Core/Service.hpp>
#include "System/InputState.hpp"
#include "System/WindowEvents.hpp"

namespace Core
{
    class ServiceStorage;
}

/*
    Input Manager

    Listens to all input related events from window and propagates them to current input state.
*/

namespace System
{
    class WindowContext;

    class InputManager final : public Core::Service
    {
        REFLECTION_ENABLE(InputManager, Core::Service)

    public:
        enum class CreateErrors
        {
        };

        using CreateResult = Common::Result<std::unique_ptr<InputManager>, CreateErrors>;
        static CreateResult Create();

    public:
        ~InputManager() override;

        void UpdateInputState(float timeDelta);
        void ResetInputState();

        InputState& GetInputState();

    private:
        InputManager();

        bool OnAttach(const Core::ServiceStorage* serviceStorage) override;

        static InputManager* GetInputManagerFromUserData(GLFWwindow* handle);
        static void TextInputCallback(GLFWwindow* handle, unsigned int character);
        static void KeyboardKeyCallback(GLFWwindow* handle, int key, int scancode, int action, int mods);
        static void MouseButtonCallback(GLFWwindow* handle, int button, int action, int mods);
        static void MouseScrollCallback(GLFWwindow* handle, double offsetx, double offsety);
        static void CursorPositionCallback(GLFWwindow* handle, double x, double y);
        static void CursorEnterCallback(GLFWwindow* handle, int entered);

        InputState m_inputState;
        WindowContext* m_windowContext = nullptr;
    };
}

REFLECTION_TYPE(System::InputManager, Core::Service)
