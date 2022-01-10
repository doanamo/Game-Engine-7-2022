/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#pragma once

#include <Common/Events/EventBroker.hpp>
#include "Platform/WindowEvents.hpp"

struct GLFWwindow;

/*
    Window

    Creates and handles a multimedia window that also manages its own OpenGL context along with
    input. Supports creation of multiple windows and OpenGL contexts.
*/

namespace Platform
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

    class Window final
    {
        REFLECTION_ENABLE(Window)

    public:
        struct CreateParams
        {
            std::string title = "Game";
            int width = 1024;
            int height = 576;
            int minWidth = -1;
            int minHeight = -1;
            int maxWidth = -1;
            int maxHeight = -1;
            bool vsync = true;
            bool visible = true;
        };

        enum class CreateErrors
        {
            InvalidSize,
            FailedWindowCreation,
            OpenGLLoaderError,
        };

        using CreateResult = Common::Result<std::unique_ptr<Window>, CreateErrors>;
        static CreateResult Create(const CreateParams& params);

    public:
        ~Window();

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
        Window();

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

REFLECTION_TYPE(Platform::Window)
