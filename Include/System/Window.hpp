/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#pragma once

#include <string>
#include <Event/Dispatcher.hpp>
#include "InputDefinitions.hpp"

/*
    Window

    Creates and handles a multimedia window that also manages its own OpenGL
    context along with input. Supports multiple windows and OpenGL contexts.
*/

namespace System
{
    class Window final : private Common::NonCopyable
    {
    public:
        struct CreateFromParams
        {
            std::string title = "Window";
            int width = 1024;
            int height = 576;
            bool vsync = true;
            bool visible = true;

            int minWidth = GLFW_DONT_CARE;
            int minHeight = GLFW_DONT_CARE;
            int maxWidth = GLFW_DONT_CARE;
            int maxHeight = GLFW_DONT_CARE;
        };

        enum class CreateErrors
        {
            InvalidArgument,
            FailedWindowCreation,
            FailedGlewInitialization,
        };

        using CreateResult = Common::Result<std::unique_ptr<Window>, CreateErrors>;
        static CreateResult Create(const CreateFromParams& params);

    public:
        ~Window();

        void MakeContextCurrent();
        void ProcessEvents();
        void Present();
        void Close();

        void SetTitle(std::string title);
        void SetVisibility(bool show);

        std::string GetTitle() const;
        int GetWidth() const;
        int GetHeight() const;
        bool IsOpen() const;
        bool IsFocused() const;

        GLFWwindow* GetPrivateHandle();

    public:
        // Publicly exposed window event dispatchers.
        struct Events
        {
            struct Move
            {
                int x;
                int y;
            };

            Event::Dispatcher<void(const Move&)> move;

            struct Resize
            {
                int width;
                int height;
            };

            Event::Dispatcher<void(const Resize&)> resize;

            struct Focus
            {
                bool focused;
            };

            Event::Dispatcher<void(const Focus&)> focus;

            struct Close
            {
            };

            Event::Dispatcher<void(const Close&)> close;

            struct KeyboardKey
            {
                int key;
                int scancode;
                int action;
                int modifiers;
            };

            Event::Dispatcher<bool(const KeyboardKey&), Event::CollectWhileFalse> keyboardKey;

            struct TextInput
            {
                // Character is stored in UTF32 format here and can be
                // converted to any other code point as the user wishes.
                unsigned int utf32Character;
            };

            Event::Dispatcher<bool(const TextInput&), Event::CollectWhileFalse> textInput;

            struct MouseButton
            {
                int button;
                int action;
                int modifiers;
            };

            Event::Dispatcher<bool(const MouseButton&), Event::CollectWhileFalse> mouseButton;

            struct MouseScroll
            {
                double offset;
            };

            Event::Dispatcher<bool(const MouseScroll&), Event::CollectWhileFalse> mouseScroll;

            struct CursorPosition
            {
                double x;
                double y;
            };

            Event::Dispatcher<void(const CursorPosition&)> cursorPosition;

            struct CursorEnter
            {
                bool entered;
            };

            Event::Dispatcher<void(const CursorEnter&)> cursorEnter;
        } events;

    private:
        Window();

        static void MoveCallback(GLFWwindow* window, int x, int y);
        static void ResizeCallback(GLFWwindow* window, int width, int height);
        static void FocusCallback(GLFWwindow* window, int focused);
        static void CloseCallback(GLFWwindow* window);
        static void KeyboardKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
        static void TextInputCallback(GLFWwindow* window, unsigned int character);
        static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
        static void MouseScrollCallback(GLFWwindow* window, double offsetx, double offsety);
        static void CursorPositionCallback(GLFWwindow* window, double x, double y);
        static void CursorEnterCallback(GLFWwindow* window, int entered);

    private:
        std::string m_title;
        GLFWwindow* m_handle = nullptr;
        bool m_sizeChanged = false;
    };
}
