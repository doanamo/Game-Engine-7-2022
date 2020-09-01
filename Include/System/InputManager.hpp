/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#pragma once

#include <Core/ServiceStorage.hpp>
#include "System/InputState.hpp"
#include "System/Window.hpp"

/*
    Input Manager

    Listens to all input related events from window and propagates them to current input state.
*/

namespace System
{
    class Window;

    class InputManager final : private Common::NonCopyable
    {
    public:
        struct CreateParams
        {
            const Core::ServiceStorage* services = nullptr;
        };

        enum class CreateErrors
        {
            InvalidArgument,
            FailedEventSubscription,
        };

        using CreateResult = Common::Result<std::unique_ptr<InputManager>, CreateErrors>;
        static CreateResult Create(const CreateParams& params);

        ~InputManager();

        void UpdateInputState(float timeDelta);
        void ResetInputState();

        InputState& GetInputState();

    private:
        InputManager();

        bool OnTextInput(const Window::Events::TextInput& event);
        bool OnKeyboardKey(const Window::Events::KeyboardKey& event);
        bool OnMouseButton(const Window::Events::MouseButton& event);
        bool OnMouseScroll(const Window::Events::MouseScroll& event);
        void OnCursorPosition(const Window::Events::CursorPosition& event);
        void OnCursorEnter(const Window::Events::CursorEnter& event);

        struct Receivers
        {
            Event::Receiver<bool(const Window::Events::TextInput&)> textInput;
            Event::Receiver<bool(const Window::Events::KeyboardKey&)> keyboardKey;
            Event::Receiver<bool(const Window::Events::MouseButton&)> mouseButton;
            Event::Receiver<bool(const Window::Events::MouseScroll&)> mouseScroll;
            Event::Receiver<void(const Window::Events::CursorPosition&)> cursorPosition;
            Event::Receiver<void(const Window::Events::CursorEnter&)> cursorEnter;
        } m_receivers;

        InputState m_inputState;
    };
}
