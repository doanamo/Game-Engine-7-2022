/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#pragma once

#include <Common/Event/EventReceiver.hpp>
#include <System/WindowEvents.hpp>
#include <System/InputDefinitions.hpp>
#include "Editor/EditorModule.hpp"

namespace System
{
    class Window;
}

/*
    Input Manager Editor

    Editor interface for inspecting input manager state.
*/

namespace Editor
{
    class InputManagerEditor final : public EditorModule
    {
        REFLECTION_ENABLE(InputManagerEditor, EditorModule)

    public:
        ~InputManagerEditor();
        InputManagerEditor();

    private:
        bool OnAttach(const Core::SystemStorage<EditorModule>& editorModules) override;
        void OnDisplay(float timeDelta) override;
        void OnDisplayMenuBar() override;

        void AddIncomingEventLog(std::string text);

        void OnWindowFocus(const System::WindowEvents::Focus& event);
        bool OnTextInput(const System::InputEvents::TextInput& event);
        bool OnKeyboardKey(const System::InputEvents::KeyboardKey& event);
        bool OnMouseButton(const System::InputEvents::MouseButton& event);
        bool OnMouseScroll(const System::InputEvents::MouseScroll& event);
        void OnCursorPosition(const System::InputEvents::CursorPosition& event);
        void OnCursorEnter(const System::InputEvents::CursorEnter& event);

        Event::Receiver<void(const System::WindowEvents::Focus&)> m_windowFocusReceiver;
        Event::Receiver<bool(const System::InputEvents::TextInput&)> m_textInputReceiver;
        Event::Receiver<bool(const System::InputEvents::KeyboardKey&)> m_keyboardKeyReceiver;
        Event::Receiver<bool(const System::InputEvents::MouseButton&)> m_mouseButtonReceiver;
        Event::Receiver<bool(const System::InputEvents::MouseScroll&)> m_mouseScrollReceiver;
        Event::Receiver<void(const System::InputEvents::CursorPosition&)> m_cursorPositionReceiver;
        Event::Receiver<void(const System::InputEvents::CursorEnter&)> m_cursorEnterReceiver;

    private:
        System::Window* m_window = nullptr;

        bool m_isOpen = false;

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

REFLECTION_TYPE(Editor::InputManagerEditor, Editor::EditorModule)
