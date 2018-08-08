/*
    Copyright (c) 2018 Piotr Doan. All rights reserved.
*/

#pragma once

#include "System/Window.hpp"

/*
    System Input State

    Caches input state between frames and handles repeating keys.
    Allows various input states to be easily polled.

    void ExampleSystemInputState(System::Window& window)
    {
        // Create an input state instance.
        System::InputState inputState;

        // Subscribe the input state to window's input events.
        inputState.PrepareForEvents(window);
    
        // Run the main window loop.
        while(window.IsOpen())
        {
            // Prepare the state for incoming events.
            inputState.Update();

            // Process events that will be dispatched to the input state.
            window.ProcessEvents();
    
            // Check if the escape key was pressed once.
            if(window.IsKeyDown(GLFW_KEY_ESCAPE, false))
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

    // Input state class.
    class InputState
    {
    public:
        // Constant variables.
        static const int KeyboardKeyCount = GLFW_KEY_LAST + 1;

        // Keyboard key states.
        struct KeyboardKeyStates
        {
            enum Type
            {
                Pressed,
                PressedRepeat,
                Released,
                ReleasedRepeat,
            };
        };

    public:
        InputState();
        ~InputState();

        // Disallow copying.
        InputState(const InputState& other) = delete;
        InputState& operator=(const InputState& other) = delete;

        // Move constructor and operator.
        InputState(InputState&& other);
        InputState& operator=(InputState&& other);

        // Initializes the input state.
        bool Initialize(Window& window);

        // Prepares the input state for incoming input events.
        // Must be called before window events are processed.
        void PrepareForEvents();

        // Resets the input state.
        void Reset();

        // Checks if a keyboard key is down.
        bool IsKeyboardKeyDown(int key, bool repeat = true);

        // Checks if a keyboard key is up.
        bool IsKeyboardKeyUp(int key, bool repeat = true);

    private:
        // Called when a keyboard key is pressed.
        void OnKeyboardKey(const Window::Events::KeyboardKey& event);

        // Called when the window changes focus.
        void OnWindowFocus(const Window::Events::Focus& event);

    private:
        // Event receivers.
        Common::Receiver<void(const Window::Events::KeyboardKey&)> m_keyboardKey;
        Common::Receiver<void(const Window::Events::Focus&)> m_windowFocus; 
        
        // States of keyboard keys.
        KeyboardKeyStates::Type m_keyboardState[KeyboardKeyCount];

        // Initialization state.
        bool m_initialized;
    };
}
