/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#pragma once

#include "System/Window.hpp"

// Forward declarations.
namespace Engine
{
    class Root;
}

/*
    Input Manager Editor
*/

namespace Editor
{
    // Input manager editor class.
    class InputManagerEditor
    {
    public:
        InputManagerEditor();
        ~InputManagerEditor();

        // Disallow copying.
        InputManagerEditor(const InputManagerEditor& other) = delete;
        InputManagerEditor& operator=(const InputManagerEditor& other) = delete;

        // Move constructor and assignment.
        InputManagerEditor(InputManagerEditor&& other);
        InputManagerEditor& operator=(InputManagerEditor&& other);

        // Initializes input manager editor.
        bool Initialize(Engine::Root* engine);

        // Updates input manager editor.
        void Update(float timeDelta);

    public:
        // Window state.
        bool mainWindowOpen;

    private:
        // Adds an incoming event log text.
        void AddIncomingEventLog(std::string text);

        // Input event handlers.
        void OnWindowFocus(const System::Window::Events::Focus& event);
        bool OnKeyboardKey(const System::Window::Events::KeyboardKey& event);
        bool OnTextInput(const System::Window::Events::TextInput& event);
        bool OnMouseButton(const System::Window::Events::MouseButton& event);
        bool OnMouseScroll(const System::Window::Events::MouseScroll& event);
        void OnCursorPosition(const System::Window::Events::CursorPosition& event);
        void OnCursorEnter(const System::Window::Events::CursorEnter& event);

    private:
        // Engine reference.
        Engine::Root* m_engine;

        // Incoming event log.
        Event::Receiver<void(const System::Window::Events::Focus&)> m_windowFocusReceiver;
        Event::Receiver<bool(const System::Window::Events::KeyboardKey&)> m_keyboardKeyReceiver;
        Event::Receiver<bool(const System::Window::Events::TextInput&)> m_textInputReceiver;
        Event::Receiver<bool(const System::Window::Events::MouseButton&)> m_mouseButtonReceiver;
        Event::Receiver<bool(const System::Window::Events::MouseScroll&)> m_mouseScrollReceiver;
        Event::Receiver<void(const System::Window::Events::CursorPosition&)> m_cursorPositionReceiver;
        Event::Receiver<void(const System::Window::Events::CursorEnter&)> m_cursorEnterReceiver;

        bool m_incomingEventFreeze;
        bool m_incomingWindowFocus;
        bool m_incomingKeyboardKey;
        bool m_incomingKeyboardKeyPress;
        bool m_incomingKeyboardKeyRelease;
        bool m_incomingKeyboardKeyRepeat;
        bool m_incomingTextInput;
        bool m_incomingMouseButton;
        bool m_incomingMouseButtonPress;
        bool m_incomingMouseButtonRelease;
        bool m_incomingMouseButtonRepeat;
        bool m_incomingMouseScroll;
        bool m_incomingCursorPosition;
        bool m_incomingCursorEnter;

        std::deque<std::string> m_incomingEventLog;
        const std::size_t m_incomingEventLogSize;
        unsigned short m_incomingEventCounter;

        // Initialization state.
        bool m_initialized;
    };
}
