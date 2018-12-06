/*
    Copyright (c) 2018 Piotr Doan. All rights reserved.
*/

#pragma once

#include "Event/Receiver.hpp"
#include "System/Window.hpp"
#include "Editor/EditorRenderer.hpp"
#include "Editor/GameStateEditor.hpp"

// Forward declarations.
namespace Engine
{
    class Root;
}

namespace System
{
    class ResourceManager;
};

namespace Game
{
    class GameState;
};

/*
    Editor System

    Displays and handles different editor interfaces.
*/

namespace Editor
{
    // Editor system class.
    class EditorSystem
    {
    public:
        EditorSystem();
        ~EditorSystem();

        // Disallow copying.
        EditorSystem(const EditorSystem& other) = delete;
        EditorSystem& operator=(const EditorSystem& other) = delete;

        // Move constructor and assignment.
        EditorSystem(EditorSystem&& other);
        EditorSystem& operator=(EditorSystem&& other);

        // Initializes the editor system.
        bool Initialize(Engine::Root* engine);

        // Updates the editor interface.
        void Update(float timeDelta);

        // Draws the editor interface.
        void Draw();

        // Gets the game state editor.
        GameStateEditor& GetGameStateEditor();

    private:
        // Callback function for cursor position events.
        void CursorPositionCallback(const System::Window::Events::CursorPosition& event);

        // Callback function for mouse button events.
        bool MouseButtonCallback(const System::Window::Events::MouseButton& event);

        // Callback function for mouse scroll events.
        bool MouseScrollCallback(const System::Window::Events::MouseScroll& event);

        // Callback function for keyboard key events.
        bool KeyboardKeyCallback(const System::Window::Events::KeyboardKey& event);

        // Callback function for text input events.
        bool TextInputCallback(const System::Window::Events::TextInput& event);

    private:
        // Destroys the user interface context.
        void DestroyContext();

    private:
        // Editor reference.
        Engine::Root* m_engine;

        // User interface context.
        ImGuiContext* m_interface;
        bool m_showDemoWindow;

        // Window event callbacks.
        Event::Receiver<void(const System::Window::Events::CursorPosition&)> m_receiverCursorPosition;
        Event::Receiver<bool(const System::Window::Events::MouseButton&)> m_receiverMouseButton;
        Event::Receiver<bool(const System::Window::Events::MouseScroll&)> m_receiverMouseScroll;
        Event::Receiver<bool(const System::Window::Events::KeyboardKey&)> m_receiverKeyboardKey;
        Event::Receiver<bool(const System::Window::Events::TextInput&)> m_receiverTextInput;

        // Editor renderer.
        EditorRenderer m_editorRenderer;
        
        // Editor systems.
        GameStateEditor m_editorGameState;

        // Initialization state.
        bool m_initialized;
    };
}
