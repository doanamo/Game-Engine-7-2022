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

    Creates and handles a multimedia window that also manages its own OpenGL
    context along with input. Supports multiple windows and OpenGL contexts.
*/

namespace System
{
    class Window;
    class InputManager;

    class WindowContext
    {
    public:
        WindowContext(Window& window) :
            window(window)
        {
        }

        GLFWwindow* GetPrivateHandle()
        {
            return handle;
        }

    private:
        friend Window;
        friend InputManager;

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

        WindowContext& GetContext();
        std::string GetTitle() const;
        int GetWidth() const;
        int GetHeight() const;
        bool ShouldClose() const;
        bool IsFocused() const;

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
    };
}

REFLECTION_TYPE(System::Window, Core::EngineSystem)
