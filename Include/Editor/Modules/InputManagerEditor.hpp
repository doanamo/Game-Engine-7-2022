/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#pragma once

#include <System/Window.hpp>

namespace Core
{
    class ServiceStorage;
}

/*
    Input Manager Editor

    Editor interface for inspecting input manager state.
*/

namespace Editor
{
    class InputManagerEditor final : private Common::NonCopyable
    {
    public:
        using WindowEvents = System::Window::Events;

        struct CreateFromParams
        {
            const Core::ServiceStorage* services = nullptr;
        };

        enum class CreateErrors
        {
            InvalidArgument,
            FailedEventSubscription,
        };

        using CreateResult = Common::Result<std::unique_ptr<InputManagerEditor>, CreateErrors>;
        static CreateResult Create(const CreateFromParams& params);

        ~InputManagerEditor();

        void Display(float timeDelta);

        bool mainWindowOpen = false;

    private:
        InputManagerEditor();

        bool SubscribeEvents();
        void AddIncomingEventLog(std::string text);

        void OnWindowFocus(const WindowEvents::Focus& event);
        bool OnTextInput(const WindowEvents::TextInput& event);
        bool OnKeyboardKey(const WindowEvents::KeyboardKey& event);
        bool OnMouseButton(const WindowEvents::MouseButton& event);
        bool OnMouseScroll(const WindowEvents::MouseScroll& event);
        void OnCursorPosition(const WindowEvents::CursorPosition& event);
        void OnCursorEnter(const WindowEvents::CursorEnter& event);

        Event::Receiver<void(const WindowEvents::Focus&)> m_windowFocusReceiver;
        Event::Receiver<bool(const WindowEvents::TextInput&)> m_textInputReceiver;
        Event::Receiver<bool(const WindowEvents::KeyboardKey&)> m_keyboardKeyReceiver;
        Event::Receiver<bool(const WindowEvents::MouseButton&)> m_mouseButtonReceiver;
        Event::Receiver<bool(const WindowEvents::MouseScroll&)> m_mouseScrollReceiver;
        Event::Receiver<void(const WindowEvents::CursorPosition&)> m_cursorPositionReceiver;
        Event::Receiver<void(const WindowEvents::CursorEnter&)> m_cursorEnterReceiver;

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
