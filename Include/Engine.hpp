/*
    Copyright (c) 2018 Piotr Doan. All rights reserved.
*/

#pragma once

#include "EnginePublic.hpp"
#include "System/Platform.hpp"
#include "System/Window.hpp"
#include "System/Timer.hpp"
#include "System/InputState.hpp"
#include "System/ResourceManager.hpp"
#include "Graphics/RenderContext.hpp"
#include "Graphics/SpriteRenderer.hpp"
#include "Game/SceneSystem.hpp"
#include "Game/SceneRenderer.hpp"
#include "Editor/EditorSystem.hpp"

/*
    Game Engine
*/

namespace Engine
{
    // Root class.
    class Root
    {
    public:
        Root();
        ~Root();

        // Disallow copying and moving.
        Root(const Root& other) = delete;
        Root& operator=(const Root& other) = delete;

        // Moving constructor and assignment.
        Root(Root&& other);
        Root& operator=(Root&& other);

        // Initializes the engine instance.
        bool Initialize();

        // Run the main game loop.
        int Run();

        // Checks if the engine instance is initialized.
        bool IsInitialized() const;

    public:
        // Platform systems.
        System::Platform platform;
        System::Window window;
        System::Timer timer;
        System::InputState inputState;
        System::ResourceManager resourceManager;

        // Graphics systems.
        Graphics::RenderContext renderContext;
        Graphics::SpriteRenderer spriteRenderer;

        // Game systems.
        Game::SceneSystem sceneSystem;
        Game::SceneRenderer sceneRenderer;

        // Engine systems.
        Editor::EditorSystem editorSystem;

    private:
        // Initialization state.
        bool m_initialized;
    };
}
