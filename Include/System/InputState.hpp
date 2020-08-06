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

        using CreateResult = Common::Result<std::unique_ptr<InputState>, void>;
        static CreateResult Create();

    public:
        ~InputState();

        void UpdateStates(float timeDelta);
        void ResetStates();

        bool IsKeyboardKeyPressed(KeyboardKeys::Type key, bool repeat = true);
        bool IsKeyboardKeyReleased(KeyboardKeys::Type key, bool repeat = true);
        bool IsMouseButtonPressed(KeyboardKeys::Type key, bool repeat = true);
        bool IsMouseButtonReleased(KeyboardKeys::Type key, bool repeat = true);

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
