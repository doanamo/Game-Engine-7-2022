/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#pragma once

#include <Common/Event/EventBroker.hpp>
#include <Core/EngineSystem.hpp>
#include "System/WindowEvents.hpp"

struct GLFWwindow;

/*
    Window

    Creates and handles a multimedia window that also manages its own OpenGL context along with
    input. Supports creation of multiple windows and OpenGL contexts.
*/

namespace System
{
    class Window;
    class InputManager;

    class WindowContext
    {
    public:
        friend Window;
        friend InputManager;

    public:
        WindowContext(Window& window)
            : window(window)
        {
        }

        GLFWwindow* GetPrivateHandle()
        {
            return handle;
        }

    private:
        Window& window;
        GLFWwindow* handle = nullptr;
        InputManager* inputManager = nullptr;
    };

    class Window final : public Core::EngineSystem
    {
        REFLECTION_ENABLE(Window, Core::EngineSystem)

    public:
        Window();
        ~Window() override;

        void MakeContextCurrent();
        void ProcessEvents();
        void Present();
        void Close();

        void SetTitle(std::string title);
        void SetVisibility(bool show);

        WindowContext& GetContext()
        {
            return m_context;
        }

        std::string GetTitle() const
        {
            return m_title;
        }

        int GetWidth() const
        {
            return m_width;
        }

        int GetHeight() const
        {
            return m_height;
        }

        bool ShouldClose() const;
        bool IsFocused() const;

    public:
        Event::Broker events;

    private:
        bool OnAttach(const Core::EngineSystemStorage& engineSystems) override;
        void OnBeginFrame() override;
        void OnEndFrame() override;

        static Window& GetWindowFromUserData(GLFWwindow* handle);
        static void MoveCallback(GLFWwindow* handle, int x, int y);
        static void ResizeCallback(GLFWwindow* handle, int width, int height);
        static void FocusCallback(GLFWwindow* handle, int focused);
        static void CloseCallback(GLFWwindow* handle);

    private:
        WindowContext m_context;

        std::string m_title;
        bool m_sizeChanged = false;
        int m_width = 0;
        int m_height = 0;
    };
}

REFLECTION_TYPE(System::Window, Core::EngineSystem)
