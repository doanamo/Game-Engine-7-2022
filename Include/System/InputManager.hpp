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

    public:
        ~InputManager();

        void SetInputState(std::shared_ptr<InputState> inputState);
        std::shared_ptr<InputState> GetInputState() const;

        void UpdateInputState(float timeDelta);
        void ResetInputState();

        struct Events
        {
            Event::Dispatcher<bool(const InputEvents::KeyboardKey&), Event::CollectWhileFalse> keyboardKey;
            Event::Dispatcher<bool(const InputEvents::TextInput&), Event::CollectWhileFalse> textInput;
            Event::Dispatcher<bool(const InputEvents::MouseButton&), Event::CollectWhileFalse> mouseButton;
            Event::Dispatcher<bool(const InputEvents::MouseScroll&), Event::CollectWhileFalse> mouseScroll;
            Event::Dispatcher<void(const InputEvents::CursorPosition&)> cursorPosition;
            Event::Dispatcher<void(const InputEvents::CursorEnter&)> cursorEnter;
        } events;

    private:
        InputManager();

        bool OnKeyboardKey(const Window::Events::KeyboardKey& event);
        bool OnTextInput(const Window::Events::TextInput& event);
        bool OnMouseButton(const Window::Events::MouseButton& event);
        bool OnMouseScroll(const Window::Events::MouseScroll& event);
        void OnCursorPosition(const Window::Events::CursorPosition& event);
        void OnCursorEnter(const Window::Events::CursorEnter& event);

        struct Receivers
        {
            Event::Receiver<bool(const Window::Events::KeyboardKey&)> keyboardKey;
            Event::Receiver<bool(const Window::Events::TextInput&)> textInput;
            Event::Receiver<bool(const Window::Events::MouseButton&)> mouseButton;
            Event::Receiver<bool(const Window::Events::MouseScroll&)> mouseScroll;
            Event::Receiver<void(const Window::Events::CursorPosition&)> cursorPosition;
            Event::Receiver<void(const Window::Events::CursorEnter&)> cursorEnter;
        } m_receivers;

        std::shared_ptr<InputState> m_inputState = nullptr;
    };
}
