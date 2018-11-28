/*
    Copyright (c) 2018 Piotr Doan. All rights reserved.
*/

#pragma once

#include "System/Platform.hpp"
#include "System/FileSystem.hpp"
#include "System/Window.hpp"
#include "System/Timer.hpp"
#include "System/InputState.hpp"
#include "System/ResourceManager.hpp"
#include "Graphics/RenderContext.hpp"
#include "Graphics/Sprite/SpriteRenderer.hpp"
#include "Renderer/StateRenderer.hpp"
#include "Editor/EditorSystem.hpp"

/*
    Engine Root
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

        // Processes one engine frame.
        bool ProcessFrame();

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

        // Renderer systems.
        Renderer::StateRenderer stateRenderer;

        // Engine systems.
        Editor::EditorSystem editorSystem;

    private:
        // Initialization state.
        bool m_initialized;
    };
}
