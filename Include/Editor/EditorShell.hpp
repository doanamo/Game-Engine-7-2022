/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#pragma once

#include "Editor/Modules/InputManagerEditor.hpp"
#include "Editor/Modules/GameStateEditor.hpp"

// Forward declarations.
namespace System
{
    class Window;
};

namespace Game
{
    class GameFramework;
}

/*
    Editor Shell
*/

namespace Editor
{
    // Editor shell class.
    class EditorShell : private NonCopyable
    {
    public:
        struct InitializeFromParams
        {
            System::Window* window = nullptr;
            Game::GameFramework* gameFramework = nullptr;
        };

    public:
        EditorShell() = default;
        ~EditorShell() = default;

        // Move constructor and assignment. 
        EditorShell(EditorShell&& other);
        EditorShell& operator=(EditorShell&& other);

        // Initializes the editor shell.
        bool Initialize(const InitializeFromParams& params);

        // Updates the interface state.
        // Must be called after ImGui::NewFrame().
        void Update(float timeDelta);

    private:
        // System references.
        System::Window* m_window = nullptr;

        // Editor modules.
        InputManagerEditor m_inputManagerEditor;
        GameStateEditor m_gameStateEditor;

        // Show ImGui demo window.
        bool m_showDemoWindow = false;

        // Initialization state.
        bool m_initialized = false;
    };
}
