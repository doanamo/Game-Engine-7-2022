/*
    Copyright (c) 2018-2019 Piotr Doan. All rights reserved.
*/

#pragma once

// Forward declarations.
namespace System
{
    class Platform;
    class FileSystem;
    class Window;
    class Timer;
    class InputManager;
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

namespace Game
{
    class EventRouter;
    class GameState;
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

        // Sets the main game state instance.
        void SetGameState(std::shared_ptr<Game::GameState>& gameState);

        // Runs the application main loop.
        int Run();

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

        // Gets the input manager.
        System::InputManager& GetInputManager();

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

        // Gets the event router.
        Game::EventRouter& GetEventRouter();

        // Gets the current game state instance.
        std::shared_ptr<Game::GameState> GetGameState();

    private:
        // Engine parameters.
        float m_maximumTickDelta;

        // Platform systems.
        std::unique_ptr<System::Platform> m_platform;
        std::unique_ptr<System::FileSystem> m_fileSystem;
        std::unique_ptr<System::Window> m_window;
        std::unique_ptr<System::Timer> m_timer;
        std::unique_ptr<System::InputManager> m_inputManager;
        std::unique_ptr<System::ResourceManager> m_resourceManager;

        // Graphics systems.
        std::unique_ptr<Graphics::RenderContext> m_renderContext;
        std::unique_ptr<Graphics::SpriteRenderer> m_spriteRenderer;

        // Renderer systems.
        std::unique_ptr<Renderer::StateRenderer> m_stateRenderer;

        // Engine systems.
        std::unique_ptr<Editor::EditorSystem> m_editorSystem;

        // Game systems.
        std::shared_ptr<Game::EventRouter> m_eventRouter;
        std::shared_ptr<Game::GameState> m_gameState;

        // Initialization state.
        bool m_initialized;
    };
}
