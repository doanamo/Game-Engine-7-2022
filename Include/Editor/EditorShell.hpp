/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#pragma once

#include "Editor/Modules/InputManagerEditor.hpp"
#include "Editor/Modules/GameStateEditor.hpp"

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

        EditorShell(EditorShell&& other);
        EditorShell& operator=(EditorShell&& other);

        bool Initialize(const InitializeFromParams& params);
        void Update(float timeDelta);

    private:
        System::Window* m_window = nullptr;

        InputManagerEditor m_inputManagerEditor;
        GameStateEditor m_gameStateEditor;

        bool m_showDemoWindow = false;
        bool m_initialized = false;
    };
}
