/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#pragma once

#include <queue>
#include <System/Window.hpp>

/*
    Input Manager Editor
*/

namespace Editor
{
    class InputManagerEditor final : private Common::NonCopyable
    {
    public:
        struct CreateFromParams
        {
            System::Window* window = nullptr;
        };

        enum class CreateErrors
        {
            InvalidArgument,
            FailedEventSubscription,
        };

        using CreateResult = Common::Result<std::unique_ptr<InputManagerEditor>, CreateErrors>;
        static CreateResult Create(const CreateFromParams& params);

    public:
        ~InputManagerEditor();

        void Update(float timeDelta);

    public:
        bool mainWindowOpen = false;

    private:
        InputManagerEditor();

        Event::Receiver<void(const System::Window::Events::Focus&)> m_windowFocusReceiver;
        Event::Receiver<bool(const System::Window::Events::KeyboardKey&)> m_keyboardKeyReceiver;
        Event::Receiver<bool(const System::Window::Events::TextInput&)> m_textInputReceiver;
        Event::Receiver<bool(const System::Window::Events::MouseButton&)> m_mouseButtonReceiver;
        Event::Receiver<bool(const System::Window::Events::MouseScroll&)> m_mouseScrollReceiver;
        Event::Receiver<void(const System::Window::Events::CursorPosition&)> m_cursorPositionReceiver;
        Event::Receiver<void(const System::Window::Events::CursorEnter&)> m_cursorEnterReceiver;

        void OnWindowFocus(const System::Window::Events::Focus& event);
        bool OnKeyboardKey(const System::Window::Events::KeyboardKey& event);
        bool OnTextInput(const System::Window::Events::TextInput& event);
        bool OnMouseButton(const System::Window::Events::MouseButton& event);
        bool OnMouseScroll(const System::Window::Events::MouseScroll& event);
        void OnCursorPosition(const System::Window::Events::CursorPosition& event);
        void OnCursorEnter(const System::Window::Events::CursorEnter& event);

        void AddIncomingEventLog(std::string text);

    private:
        System::Window* m_window = nullptr;

        bool m_incomingEventFreeze = false;
        bool m_incomingWindowFocus = false;
        bool m_incomingKeyboardKey = false;
        bool m_incomingKeyboardKeyPress = false;
        bool m_incomingKeyboardKeyRelease = false;
        bool m_incomingKeyboardKeyRepeat = false;
        bool m_incomingTextInput = false;
        bool m_incomingMouseButton = false;
        bool m_incomingMouseButtonPress = false;
        bool m_incomingMouseButtonRelease = false;
        bool m_incomingMouseButtonRepeat = false;
        bool m_incomingMouseScroll = false;
        bool m_incomingCursorPosition = false;
        bool m_incomingCursorEnter = false;

        std::deque<std::string> m_incomingEventLog;
        const std::size_t m_incomingEventLogSize = 100;
        unsigned short m_incomingEventCounter = 0;
    };
}
