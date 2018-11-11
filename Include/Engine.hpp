/*
    Copyright (c) 2018 Piotr Doan. All rights reserved.
*/

#pragma once

#include "EnginePublic.hpp"
#include "System/Platform.hpp"
#include "System/FileSystem.hpp"
#include "System/Window.hpp"
#include "System/Timer.hpp"
#include "System/InputState.hpp"
#include "System/ResourceManager.hpp"
#include "Graphics/RenderContext.hpp"
#include "Graphics/Sprite/SpriteRenderer.hpp"
#include "Scene/SceneSystem.hpp"
#include "Scene/SceneRenderer.hpp"
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
        System::FileSystem fileSystem;
        System::Window window;
        System::Timer timer;
        System::InputState inputState;
        System::ResourceManager resourceManager;

        // Graphics systems.
        Graphics::RenderContext renderContext;
        Graphics::SpriteRenderer spriteRenderer;

        // Scene systems.
        Scene::SceneSystem sceneSystem;
        Scene::SceneRenderer sceneRenderer;

        // Engine systems.
        Editor::EditorSystem editorSystem;

    private:
        // Initialization state.
        bool m_initialized;
    };
}
