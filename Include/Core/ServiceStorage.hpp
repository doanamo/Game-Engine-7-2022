/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#pragma once

#include <memory>

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
    Service Storage
*/

namespace Core
{
    class ServiceStorage
    {
    public:
        ServiceStorage();
        ~ServiceStorage();

        void Provide(std::unique_ptr<System::Platform>&& platform);
        void Provide(std::unique_ptr<System::FileSystem>&& fileSystem);
        void Provide(std::unique_ptr<System::Window>&& window);
        void Provide(std::unique_ptr<System::Timer>&& timer);
        void Provide(std::unique_ptr<System::InputManager>&& inputManager);
        void Provide(std::unique_ptr<System::ResourceManager>&& resourceManager);
        void Provide(std::unique_ptr<Graphics::RenderContext>&& renderContext);
        void Provide(std::unique_ptr<Graphics::SpriteRenderer>&& spriteRenderer);
        void Provide(std::unique_ptr<Renderer::StateRenderer>&& stateRenderer);
        void Provide(std::unique_ptr<Game::GameFramework>&& gameFramework);
        void Provide(std::unique_ptr<Editor::EditorSystem>&& editorSystem);

        System::Platform* GetPlatform() const;
        System::FileSystem* GetFileSystem() const;
        System::Window* GetWindow() const;
        System::Timer* GetTimer() const;
        System::InputManager* GetInputManager() const;
        System::ResourceManager* GetResourceManager() const;
        Graphics::RenderContext* GetRenderContext() const;
        Graphics::SpriteRenderer* GetSpriteRenderer() const;
        Renderer::StateRenderer* GetStateRenderer() const;
        Game::GameFramework* GetGameFramework() const;
        Editor::EditorSystem* GetEditorSystem() const;

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
    };
}
