/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#pragma once

#include <Core/ServiceStorage.hpp>
#include "System/InputState.hpp"
#include "System/WindowEvents.hpp"

/*
    Input Manager

    Listens to all input related events from window and propagates them to current input state.
*/

namespace System
{
    class WindowContext;

    class InputManager final : private Common::NonCopyable
    {
    public:
        struct CreateParams
        {
            const Core::ServiceStorage* services = nullptr;
        };

        enum class CreateErrors
        {
            InvalidArgument,
            InvalidWindow,
        };

        using CreateResult = Common::Result<std::unique_ptr<InputManager>, CreateErrors>;
        static CreateResult Create(const CreateParams& params);

        ~InputManager();

        void UpdateInputState(float timeDelta);
        void ResetInputState();

        InputState& GetInputState();

    private:
        InputManager();

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
