/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#pragma once

#include <Event/Receiver.hpp>
#include <System/InputDefinitions.hpp>
#include "Editor/EditorRenderer.hpp"
#include "Editor/EditorShell.hpp"

// Forward declarations.
namespace System
{
    class FileSystem;
    class ResourceManager;
    class InputManager;
    class Window;
};

namespace Graphics
{
    class RenderContext;
}

namespace Game
{
    class GameFramework;
};

/*
    Editor System

    Displays and handles different editor interfaces.
*/

namespace Editor
{
    // Editor system class.
    class EditorSystem : private NonCopyable
    {
    public:
        struct InitializeFromParams
        {
            System::FileSystem* fileSystem = nullptr;
            System::ResourceManager* resourceManager = nullptr;
            System::InputManager* inputManager = nullptr;
            System::Window* window = nullptr;
            Graphics::RenderContext* renderContext = nullptr;
            Game::GameFramework* gameFramework = nullptr;
        };

    public:
        EditorSystem();
        ~EditorSystem();

        // Move constructor and assignment.
        EditorSystem(EditorSystem&& other);
        EditorSystem& operator=(EditorSystem&& other);

        // Initializes the editor system.
        bool Initialize(const InitializeFromParams& params);

        // Updates the editor interface.
        void Update(float timeDelta);

        // Draws the editor interface.
        void Draw();

    private:
        // Callback function for cursor position events.
        void CursorPositionCallback(const System::InputEvents::CursorPosition& event);

        // Callback function for mouse button events.
        bool MouseButtonCallback(const System::InputEvents::MouseButton& event);

        // Callback function for mouse scroll events.
        bool MouseScrollCallback(const System::InputEvents::MouseScroll& event);

        // Callback function for keyboard key events.
        bool KeyboardKeyCallback(const System::InputEvents::KeyboardKey& event);

        // Callback function for text input events.
        bool TextInputCallback(const System::InputEvents::TextInput& event);

    private:
        // Destroys the user interface context.
        void DestroyContext();

    private:
        // User interface context.
        ImGuiContext* m_interface = nullptr;

        // Window event callbacks.
        Event::Receiver<void(const System::InputEvents::CursorPosition&)> m_receiverCursorPosition;
        Event::Receiver<bool(const System::InputEvents::MouseButton&)> m_receiverMouseButton;
        Event::Receiver<bool(const System::InputEvents::MouseScroll&)> m_receiverMouseScroll;
        Event::Receiver<bool(const System::InputEvents::KeyboardKey&)> m_receiverKeyboardKey;
        Event::Receiver<bool(const System::InputEvents::TextInput&)> m_receiverTextInput;

        // Editor systems.
        EditorRenderer m_editorRenderer;
        EditorShell m_editorShell;

        // Initialization state.
        bool m_initialized = false;
    };
}
