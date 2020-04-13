/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#pragma once

#include <Event/Dispatcher.hpp>
#include <Core/Core.hpp>

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

namespace Game
{
    class GameFramework;
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
    struct InitializeFromParams
    {
        float maximumTickDelta = 1.0f;
    };

    class Root : private NonCopyable
    {
    public:
        Root();
        ~Root();

        Root(Root&& other);
        Root& operator=(Root&& other);

        bool IsInitialized() const;
        bool Initialize(const InitializeFromParams& initParams = InitializeFromParams());
        int Run();

        System::Platform& GetPlatform();
        System::FileSystem& GetFileSystem();
        System::Window& GetWindow();
        System::Timer& GetTimer();
        System::InputManager& GetInputManager();
        System::ResourceManager& GetResourceManager();
        Graphics::RenderContext& GetRenderContext();
        Graphics::SpriteRenderer& GetSpriteRenderer();
        Renderer::StateRenderer& GetStateRenderer();
        Game::GameFramework& GetGameFramework();
        Editor::EditorSystem& GetEditorSystem();

    private:
        std::unique_ptr<System::Platform> m_platform;
        std::unique_ptr<System::FileSystem> m_fileSystem;
        std::unique_ptr<System::Window> m_window;
        std::unique_ptr<System::Timer> m_timer;
        std::unique_ptr<System::InputManager> m_inputManager;
        std::unique_ptr<System::ResourceManager> m_resourceManager;
        std::unique_ptr<Graphics::RenderContext> m_renderContext;
        std::unique_ptr<Graphics::SpriteRenderer> m_spriteRenderer;
        std::unique_ptr<Renderer::StateRenderer> m_stateRenderer;
        std::unique_ptr<Game::GameFramework> m_gameFramework;
        std::unique_ptr<Editor::EditorSystem> m_editorSystem;

        float m_maximumTickDelta;
        bool m_initialized;
    };
}
