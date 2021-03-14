/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#pragma once

#include <Core/ServiceStorage.hpp>
#include "System/InputState.hpp"
#include "System/WindowEvents.hpp"

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

        bool OnTextInput(const WindowEvents::TextInput& event);
        bool OnKeyboardKey(const WindowEvents::KeyboardKey& event);
        bool OnMouseButton(const WindowEvents::MouseButton& event);
        bool OnMouseScroll(const WindowEvents::MouseScroll& event);
        void OnCursorPosition(const WindowEvents::CursorPosition& event);
        void OnCursorEnter(const WindowEvents::CursorEnter& event);

        struct Receivers
        {
            Event::Receiver<bool(const WindowEvents::TextInput&)> textInput;
            Event::Receiver<bool(const WindowEvents::KeyboardKey&)> keyboardKey;
            Event::Receiver<bool(const WindowEvents::MouseButton&)> mouseButton;
            Event::Receiver<bool(const WindowEvents::MouseScroll&)> mouseScroll;
            Event::Receiver<void(const WindowEvents::CursorPosition&)> cursorPosition;
            Event::Receiver<void(const WindowEvents::CursorEnter&)> cursorEnter;
        } m_receivers;

        InputState m_inputState;
    };
}
