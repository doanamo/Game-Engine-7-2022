/*
    Copyright (c) 2018-2019 Piotr Doan. All rights reserved.
*/

#pragma once

#include "System/Window.hpp"
#include "System/InputDefinitions.hpp"

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
            inputSystem.PrepareForEvents();

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
    // Forward declarations.
    class Window;

    // Input manager class.
    class InputManager
    {
    public:
        InputManager();
        ~InputManager();

        // Disallow copying.
        InputManager(const InputManager& other) = delete;
        InputManager& operator=(const InputManager& other) = delete;

        // Move constructor and operator.
        InputManager(InputManager&& other);
        InputManager& operator=(InputManager&& other);

        // Initializes the input manager.
        bool Initialize(Window* window);

        // Prepares the input manager for incoming input events.
        // Must be called before window events are processed.
        void PrepareForEvents();

    public:
        // Event dispatchers.
        struct Events
        {
            // Input event dispatchers.
            Event::Dispatcher<bool(const InputEvents::KeyboardKey&), Event::CollectWhileFalse> keyboardKey;
            Event::Dispatcher<bool(const InputEvents::TextInput&), Event::CollectWhileFalse> textInput;
            Event::Dispatcher<bool(const InputEvents::MouseButton&), Event::CollectWhileFalse> mouseButton;
            Event::Dispatcher<bool(const InputEvents::MouseScroll&), Event::CollectWhileFalse> mouseScroll;
            Event::Dispatcher<void(const InputEvents::CursorPosition&)> cursorPosition;
            Event::Dispatcher<void(const InputEvents::CursorEnter&)> cursorEnter;
        } events;

    private:
        // Input event receivers.
        struct Receivers
        {
            Event::Receiver<bool(const Window::Events::KeyboardKey&)> keyboardKey;
            Event::Receiver<bool(const Window::Events::TextInput&)> textInput;
            Event::Receiver<bool(const Window::Events::MouseButton&)> mouseButton;
            Event::Receiver<bool(const Window::Events::MouseScroll&)> mouseScroll;
            Event::Receiver<void(const Window::Events::CursorPosition&)> cursorPosition;
            Event::Receiver<void(const Window::Events::CursorEnter&)> cursorEnter;
        } m_receivers;

        // Input event handlers.
        bool OnKeyboardKey(const Window::Events::KeyboardKey& event);
        bool OnTextInput(const Window::Events::TextInput& event);
        bool OnMouseButton(const Window::Events::MouseButton& event);
        bool OnMouseScroll(const Window::Events::MouseScroll& event);
        void OnCursorPosition(const Window::Events::CursorPosition& event);
        void OnCursorEnter(const Window::Events::CursorEnter& event);

    private:
        // Initialization state.
        bool m_initialized;
    };
}
