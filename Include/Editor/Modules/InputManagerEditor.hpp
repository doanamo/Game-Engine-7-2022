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

    public:
        ~InputManagerEditor();

        void Display(float timeDelta);

        bool mainWindowOpen = false;

    private:
        InputManagerEditor();

        bool SubscribeEvents();
        void AddIncomingEventLog(std::string text);

        void OnWindowFocus(const System::WindowEvents::Focus& event);
        bool OnTextInput(const System::WindowEvents::TextInput& event);
        bool OnKeyboardKey(const System::WindowEvents::KeyboardKey& event);
        bool OnMouseButton(const System::WindowEvents::MouseButton& event);
        bool OnMouseScroll(const System::WindowEvents::MouseScroll& event);
        void OnCursorPosition(const System::WindowEvents::CursorPosition& event);
        void OnCursorEnter(const System::WindowEvents::CursorEnter& event);

        Event::Receiver<void(const System::WindowEvents::Focus&)> m_windowFocusReceiver;
        Event::Receiver<bool(const System::WindowEvents::TextInput&)> m_textInputReceiver;
        Event::Receiver<bool(const System::WindowEvents::KeyboardKey&)> m_keyboardKeyReceiver;
        Event::Receiver<bool(const System::WindowEvents::MouseButton&)> m_mouseButtonReceiver;
        Event::Receiver<bool(const System::WindowEvents::MouseScroll&)> m_mouseScrollReceiver;
        Event::Receiver<void(const System::WindowEvents::CursorPosition&)> m_cursorPositionReceiver;
        Event::Receiver<void(const System::WindowEvents::CursorEnter&)> m_cursorEnterReceiver;

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
