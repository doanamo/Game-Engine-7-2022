/*
    Copyright (c) 2018-2019 Piotr Doan. All rights reserved.
*/

#pragma once

#include "Editor/Modules/InputManagerEditor.hpp"
#include "Editor/Modules/GameStateEditor.hpp"

// Forward declarations.
namespace Engine
{
    class Root;
}

/*
    Editor Shell
*/

namespace Editor
{
    // Editor shell class.
    class EditorShell
    {
    public:
        EditorShell();
        ~EditorShell();

        // Disallow copying.
        EditorShell(const EditorShell& other) = delete;
        EditorShell& operator=(const EditorShell& other) = delete;

        // Move constructor and assignment. 
        EditorShell(EditorShell&& other);
        EditorShell& operator=(EditorShell&& other);

        // Initializes the editor shell.
        bool Initialize(Engine::Root* engine);

        // Updates the interface state.
        // Must be called after ImGui::NewFrame().
        void Update(float timeDelta);

    private:
        // Engine reference.
        Engine::Root* m_engine;

        // Editor modules.
        InputManagerEditor m_inputManagerEditor;
        GameStateEditor m_gameStateEditor;

        // Show ImGui demo window.
        bool m_showDemoWindow;

        // Initialization state.
        bool m_initialized;
    };
}
