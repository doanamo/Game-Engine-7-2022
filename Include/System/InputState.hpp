/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#pragma once

#include "Event/Dispatcher.hpp"
#include "System/InputDefinitions.hpp"

/*
    Input State
*/

namespace System
{
    class InputManager;

    class InputState final : private Common::NonCopyable
    {
    public:
        friend InputManager;

        ~InputState();

        bool IsKeyboardKeyPressed(KeyboardKeys::Type key, bool repeat = true) const;
        bool IsKeyboardKeyReleased(KeyboardKeys::Type key, bool repeat = true) const;
        bool IsMouseButtonPressed(KeyboardKeys::Type key, bool repeat = true) const;
        bool IsMouseButtonReleased(KeyboardKeys::Type key, bool repeat = true) const;

        struct Events
        {
            Event::Dispatcher<bool(const InputEvents::TextInput&), Event::CollectWhileFalse> textInput;
            Event::Dispatcher<bool(const InputEvents::KeyboardKey&), Event::CollectWhileFalse> keyboardKey;
            Event::Dispatcher<bool(const InputEvents::MouseButton&), Event::CollectWhileFalse> mouseButton;
            Event::Dispatcher<bool(const InputEvents::MouseScroll&), Event::CollectWhileFalse> mouseScroll;
            Event::Dispatcher<void(const InputEvents::CursorPosition&)> cursorPosition;
            Event::Dispatcher<void(const InputEvents::CursorEnter&)> cursorEnter;
        } events;

    private:
        InputState();

        void UpdateStates(float timeDelta);
        void ResetStates();

        bool OnTextInput(const InputEvents::TextInput& event);
        bool OnKeyboardKey(const InputEvents::KeyboardKey& event);
        bool OnMouseButton(const InputEvents::MouseButton& event);
        bool OnMouseScroll(const InputEvents::MouseScroll& event);
        void OnCursorPosition(const InputEvents::CursorPosition& event);
        void OnCursorEnter(const InputEvents::CursorEnter& event);

        InputEvents::KeyboardKey m_keyboardKeyStates[KeyboardKeys::Count];
        InputEvents::MouseButton m_mouseButtonStates[MouseButtons::Count];
    };
}
