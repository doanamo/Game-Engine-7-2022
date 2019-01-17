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

    private:
        // Called when a keyboard key is pressed.
        bool OnKeyboardKey(const Window::Events::KeyboardKey& event);

        // Called when the window changes focus.
        void OnWindowFocus(const Window::Events::Focus& event);

    private:
        // Event receivers.
        Event::Receiver<bool(const Window::Events::KeyboardKey&)> m_keyboardKey;
        Event::Receiver<void(const Window::Events::Focus&)> m_windowFocus;
        
        // Initialization state.
        bool m_initialized;
    };
}
