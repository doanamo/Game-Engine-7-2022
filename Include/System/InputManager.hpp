/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#pragma once

#include <Common/Event/EventReceiver.hpp>
#include <Core/EngineSystem.hpp>
#include "System/InputState.hpp"
#include "System/WindowEvents.hpp"

namespace System
{
    class Timer;
}

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

        void UpdateInputState();
        void ResetInputState();

        InputState& GetInputState();

    public:
        struct Events
        {
            // Should be dispatched when tick is processed in order to update input state.
            Event::Receiver<void(float)> onTickProcessed;
        } events;

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
        System::Timer* m_timer = nullptr;
        WindowContext* m_windowContext = nullptr;
        InputState m_inputState;
    };
}

REFLECTION_TYPE(System::InputManager, Core::EngineSystem)
