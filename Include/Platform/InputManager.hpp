/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#pragma once

#include <Common/Events/EventReceiver.hpp>
#include <Core/System/EngineSystem.hpp>
#include "Platform/InputState.hpp"
#include "Platform/WindowEvents.hpp"

namespace Platform
{
    class TimerSystem;
}

/*
    Input Manager

    Listens to all input related events from window and propagates them to current input state.
*/

namespace Platform
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

        InputState& GetInputState()
        {
            return m_inputState;
        }

    public:
        struct Events
        {
            // Should be received when tick is processed in order to update input state.
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
        Platform::TimerSystem* m_timerSystem = nullptr;
        WindowContext* m_windowContext = nullptr;
        InputState m_inputState;
    };
}

REFLECTION_TYPE(Platform::InputManager, Core::EngineSystem)
