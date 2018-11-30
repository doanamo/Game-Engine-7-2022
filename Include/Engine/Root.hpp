/*
    Copyright (c) 2018 Piotr Doan. All rights reserved.
*/

#pragma once

// Forward declarations.
namespace System
{
    class Platform;
    class FileSystem;
    class Window;
    class Timer;
    class InputState;
    class ResourceManager;
}

namespace Graphics
{
    class RenderContext;
    class SpriteRenderer;
}

namespace Renderer
{
    class StateRenderer;
}

namespace Editor
{
    class EditorSystem;
}

/*
    Engine Root
*/

namespace Engine
{
    // Initialization params.
    struct InitializeParams
    {
        InitializeParams();

        float maximumTickDelta;
    };

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
        bool Initialize(const InitializeParams& initParams = InitializeParams());

        // Processes one engine frame.
        bool ProcessFrame();

        // Checks if the engine instance is initialized.
        bool IsInitialized() const;

    public:
        // Gets the system platform.
        System::Platform& GetPlatform();

        // Gets the file system.
        System::FileSystem& GetFileSystem();

        // Gets the window.
        System::Window& GetWindow();

        // Gets the timer.
        System::Timer& GetTimer();

        // Gets the input state.
        System::InputState& GetInputState();

        // Gets the resource manager.
        System::ResourceManager& GetResourceManager();

        // Gets the render context.
        Graphics::RenderContext& GetRenderContext();

        // Gets the sprite renderer.
        Graphics::SpriteRenderer& GetSpriteRenderer();

        // Gets the state renderer.
        Renderer::StateRenderer& GetStateRenderer();

        // Gets the editor system.
        Editor::EditorSystem& GetEditorSystem();

    private:
        // Engine parameters.
        float m_maximumTickDelta;

        // Platform systems.
        std::unique_ptr<System::Platform> m_platform;
        std::unique_ptr<System::FileSystem> m_fileSystem;
        std::unique_ptr<System::Window> m_window;
        std::unique_ptr<System::Timer> m_timer;
        std::unique_ptr<System::InputState> m_inputState;
        std::unique_ptr<System::ResourceManager> m_resourceManager;

        // Graphics systems.
        std::unique_ptr<Graphics::RenderContext> m_renderContext;
        std::unique_ptr<Graphics::SpriteRenderer> m_spriteRenderer;

        // Renderer systems.
        std::unique_ptr<Renderer::StateRenderer> m_stateRenderer;

        // Engine systems.
        std::unique_ptr<Editor::EditorSystem> m_editorSystem;

        // Initialization state.
        bool m_initialized;
    };
}
