/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#pragma once

#include "Window.hpp"
#include "InputDefinitions.hpp"

/*
    Input Manager

    Listens to all input related events and propagates them further.

    void ExampleSystemInputSystem(System::Window& window)
    {
        // Create an input manager instance.
        System::InputManager inputSystem;

        // Subscribe the input system to window's input events.
        inputSystem.Initialize(window);
    
        // Run the main window loop.
        while(window.IsOpen())
        {
            // Prepare the state for incoming events.
            inputSystem.AdvanceState();

            // Process events that will be dispatched to the input state.
            window.ProcessEvents();

            // Check if the escape key was pressed once.
            if(inputSystem.IsKeyDown(GLFW_KEY_ESCAPE, false))
            {
                Log() << "Escape key has been pressed!";
            }
        }
    }
*/

namespace System
{
    class Window;

    class InputManager final : private NonCopyable, public Resettable<InputManager>
    {
    public:
        enum class InitializeErrors
        {
            InvalidArgument,
            FailedSubscription,
        };

        using InitializeResult = Result<void, InitializeErrors>;

    public:
        InputManager();
        ~InputManager();

        InitializeResult Initialize(Window* window);
        void AdvanceState(float timeDelta);
        void ResetStates();

        bool IsKeyboardKeyPressed(KeyboardKeys::Type key, bool repeat = true);
        bool IsKeyboardKeyReleased(KeyboardKeys::Type key, bool repeat = true);

    public:
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
        struct Receivers
        {
            Event::Receiver<bool(const Window::Events::KeyboardKey&)> keyboardKey;
            Event::Receiver<bool(const Window::Events::TextInput&)> textInput;
            Event::Receiver<bool(const Window::Events::MouseButton&)> mouseButton;
            Event::Receiver<bool(const Window::Events::MouseScroll&)> mouseScroll;
            Event::Receiver<void(const Window::Events::CursorPosition&)> cursorPosition;
            Event::Receiver<void(const Window::Events::CursorEnter&)> cursorEnter;
        } m_receivers;

        bool OnKeyboardKey(const Window::Events::KeyboardKey& event);
        bool OnTextInput(const Window::Events::TextInput& event);
        bool OnMouseButton(const Window::Events::MouseButton& event);
        bool OnMouseScroll(const Window::Events::MouseScroll& event);
        void OnCursorPosition(const Window::Events::CursorPosition& event);
        void OnCursorEnter(const Window::Events::CursorEnter& event);

    private:
        InputEvents::KeyboardKey m_keyboardKeyStates[KeyboardKeys::Count];
        bool m_initialized = false;
    };
}
