/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#pragma once

#include <Common/Events/EventReceiver.hpp>
#include <Platform/WindowEvents.hpp>
#include <Platform/InputDefinitions.hpp>
#include "Editor/EditorModule.hpp"

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

        void OnWindowFocus(const Platform::WindowEvents::Focus& event);
        bool OnTextInput(const Platform::InputEvents::TextInput& event);
        bool OnKeyboardKey(const Platform::InputEvents::KeyboardKey& event);
        bool OnMouseButton(const Platform::InputEvents::MouseButton& event);
        bool OnMouseScroll(const Platform::InputEvents::MouseScroll& event);
        void OnCursorPosition(const Platform::InputEvents::CursorPosition& event);
        void OnCursorEnter(const Platform::InputEvents::CursorEnter& event);

        Event::Receiver<void(const Platform::WindowEvents::Focus&)> m_windowFocusReceiver;
        Event::Receiver<bool(const Platform::InputEvents::TextInput&)> m_textInputReceiver;
        Event::Receiver<bool(const Platform::InputEvents::KeyboardKey&)> m_keyboardKeyReceiver;
        Event::Receiver<bool(const Platform::InputEvents::MouseButton&)> m_mouseButtonReceiver;
        Event::Receiver<bool(const Platform::InputEvents::MouseScroll&)> m_mouseScrollReceiver;
        Event::Receiver<void(const Platform::InputEvents::CursorPosition&)> m_cursorPositionReceiver;
        Event::Receiver<void(const Platform::InputEvents::CursorEnter&)> m_cursorEnterReceiver;

    private:
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
