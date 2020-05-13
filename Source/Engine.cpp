/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#include "Precompiled.hpp"
#include "Engine.hpp"
#include <Common/Build.hpp>
#include <System/Platform.hpp>
#include <System/Timer.hpp>
#include <System/FileSystem.hpp>
#include <System/ResourceManager.hpp>
#include <System/InputManager.hpp>
#include <System/Window.hpp>
#include <Graphics/RenderContext.hpp>
#include <Graphics/Sprite/SpriteRenderer.hpp>
#include <Renderer/StateRenderer.hpp>
#include <Game/GameFramework.hpp>
#include <Editor/EditorSystem.hpp>
using namespace Engine;

Root::Root() = default;
Root::~Root() = default;

Root::CreateResult Root::Create(const CreateFromParams& params)
{
    LOG_INFO("Creating engine...");
    LOG_SCOPED_INDENT();

    // Check arguments.
    CHECK_ARGUMENT_OR_RETURN(params.maximumTickDelta > 0.0f, Common::Failure(CreateErrors::InvalidArgument));

    // Initialize various debugging helpers.
    Debug::Initialize();

    // Initialize default logger output.
    Logger::Initialize();

    // Initialize information acquired from the build system.
    Build::Initialize();
    Build::PrintInfo();

    // Create instance.
    auto instance = std::unique_ptr<Root>(new Root());

    // Create system platform context.
    // Allows us to use platform systems such as window or input.
    instance->m_platform = System::Platform::Create().UnwrapOr(nullptr);
    if(instance->m_platform == nullptr)
    {
        LOG_ERROR("Could not create platform!");
        return Common::Failure(CreateErrors::FailedSubsystemCreation);
    }

    // Create file system.
    // Allows path resolving with multiple mounted directories.
    instance->m_fileSystem = System::FileSystem::Create().UnwrapOr(nullptr);
    if(instance->m_fileSystem == nullptr)
    {
        LOG_ERROR("Could not create file system!");
        return Common::Failure(CreateErrors::FailedSubsystemCreation);
    }

    // Mount file system directories (order affects the resolve order).
    if(!Build::GetEngineDir().empty())
    {
        instance->m_fileSystem->MountDirectory(Build::GetEngineDir());
    }

    if(!Build::GetGameDir().empty())
    {
        instance->m_fileSystem->MountDirectory(Build::GetGameDir());
    }

    // Create main window.
    // We will be collecting input and then drawing into this window.
    // Window instance will create unique OpenGL context for us.
    System::Window::CreateFromParams windowParams;
    windowParams.title = "Game";
    windowParams.width = 1024;
    windowParams.height = 576;
    windowParams.vsync = true;
    windowParams.visible = true;

    instance->m_window = System::Window::Create(windowParams).UnwrapOr(nullptr);
    if(instance->m_window == nullptr)
    {
        LOG_ERROR("Could not create window!");
        return Common::Failure(CreateErrors::FailedSubsystemCreation);
    }

    // Create timer.
    // There can be many timers but this one will be used to calculate frame time.
    instance->m_timer = System::Timer::Create().UnwrapOr(nullptr);
    if(instance->m_timer == nullptr)
    {
        LOG_ERROR("Could not create timer!");
        return Common::Failure(CreateErrors::FailedSubsystemCreation);
    }

    instance->m_maximumTickDelta = params.maximumTickDelta;

    // Create input manager.
    // Collects and routes all events related to input.
    instance->m_inputManager = System::InputManager::Create(instance->m_window.get()).UnwrapOr(nullptr);
    if(instance->m_inputManager == nullptr)
    {
        LOG_ERROR("Could not create input manager!");
        return Common::Failure(CreateErrors::FailedSubsystemCreation);
    }

    // Create resource manager.
    // Help avoid duplication of loaded resources.
    instance->m_resourceManager = System::ResourceManager::Create().UnwrapOr(nullptr);
    if(instance->m_resourceManager == nullptr)
    {
        LOG_ERROR("Could not create resource manager!");
        return Common::Failure(CreateErrors::FailedSubsystemCreation);
    }

    // Create render context.
    // Manages rendering context created along with window.
    instance->m_renderContext = Graphics::RenderContext::Create(instance->m_window.get()).UnwrapOr(nullptr);
    if(instance->m_renderContext == nullptr)
    {
        LOG_ERROR("Could not create render context!");
        return Common::Failure(CreateErrors::FailedSubsystemCreation);
    }

    // Create sprite renderer.
    // Rendering subsystem for drawing sprites.
    Graphics::SpriteRenderer::CreateFromParams spriteRendererParams;
    spriteRendererParams.fileSystem = instance->m_fileSystem.get();
    spriteRendererParams.resourceManager = instance->m_resourceManager.get();
    spriteRendererParams.renderContext = instance->m_renderContext.get();
    spriteRendererParams.spriteBatchSize = 128;

    instance->m_spriteRenderer = Graphics::SpriteRenderer::Create(spriteRendererParams).UnwrapOr(nullptr);
    if(instance->m_spriteRenderer == nullptr)
    {
        LOG_ERROR("Could not create sprite renderer!");
        return Common::Failure(CreateErrors::FailedSubsystemCreation);
    }

    // Create state renderer.
    // Renders game state described in its components.
    Renderer::StateRenderer::CreateFromParams stateRendererParams;
    stateRendererParams.renderContext = instance->m_renderContext.get();
    stateRendererParams.spriteRenderer = instance->m_spriteRenderer.get();

    instance->m_stateRenderer = Renderer::StateRenderer::Create(stateRendererParams).UnwrapOr(nullptr);
    if(instance->m_stateRenderer == nullptr)
    {
        LOG_ERROR("Could not create state renderer!");
        return Common::Failure(CreateErrors::FailedSubsystemCreation);
    }

    // Create game framework.
    // Controls how game state is managed and how it interacts with the rest of the engine.
    Game::GameFramework::CreateFromParams gameFrameworkParams;
    gameFrameworkParams.timer = instance->m_timer.get();
    gameFrameworkParams.inputManager = instance->m_inputManager.get();
    gameFrameworkParams.window = instance->m_window.get();
    gameFrameworkParams.stateRenderer = instance->m_stateRenderer.get();

    instance->m_gameFramework = Game::GameFramework::Create(gameFrameworkParams).UnwrapOr(nullptr);
    if(instance->m_gameFramework == nullptr)
    {
        LOG_ERROR("Could not create game framework!");
        return Common::Failure(CreateErrors::FailedSubsystemCreation);
    }

    // Create editor system.
    // Built in editor for creating and modifying content within game.
    Editor::EditorSystem::CreateFromParams editorSystemParams;
    editorSystemParams.fileSystem = instance->m_fileSystem.get();
    editorSystemParams.resourceManager = instance->m_resourceManager.get();
    editorSystemParams.inputManager = instance->m_inputManager.get();
    editorSystemParams.window = instance->m_window.get();
    editorSystemParams.renderContext = instance->m_renderContext.get();
    editorSystemParams.gameFramework = instance->m_gameFramework.get();

    instance->m_editorSystem = Editor::EditorSystem::Create(editorSystemParams).UnwrapOr(nullptr);
    if(instance->m_editorSystem == nullptr)
    {
        LOG_ERROR("Could not create editor system!");
        return Common::Failure(CreateErrors::FailedSubsystemCreation);
    }

    // Success!
    return Common::Success(std::move(instance));
}

int Root::Run()
{
    // Reset time that has accumulated during initialization.
    m_timer->Reset();

    // Define main loop iteration.
    auto mainLoopIteration = [](void* engine)
    {
        // Retrieve engine root.
        ASSERT(engine != nullptr);
        Root& root = *static_cast<Root*>(engine);

        // Advance logger's frame of reference.
        Logger::AdvanceFrameReference();

        // Release unused resources.
        root.m_resourceManager->ReleaseUnused();

        // Tick timer.
        root.m_timer->Tick(root.m_maximumTickDelta);

        // Calculate frame delta time.
        float timeDelta = root.m_timer->GetDeltaSeconds();

        // Process window events.
        root.m_window->ProcessEvents();

        // Update editor system.
        root.m_editorSystem->Update(timeDelta);

        // Update game state.
        if(root.m_gameFramework->Update())
        {
            // Prepare input manager for incoming events.
            root.m_inputManager->AdvanceState(timeDelta);
        }

        // Draw game state.
        root.m_gameFramework->Draw();

        // Draw editor system.
        root.m_editorSystem->Draw();

        // Present window content.
        root.m_window->Present();
    };

    // Run main loop.
    #ifndef __EMSCRIPTEN__
        while(m_window->IsOpen())
        {
            mainLoopIteration(this);
        }
    #else
        emscripten_set_main_loop_arg(mainLoopIteration, this, 0, 1);
    #endif

    // Return error code.
    return 0;
}

System::Platform& Root::GetPlatform()
{
    ASSERT(m_platform);
    return *m_platform;
}

System::FileSystem& Root::GetFileSystem()
{
    ASSERT(m_fileSystem);
    return *m_fileSystem;
}

System::Window& Root::GetWindow()
{
    ASSERT(m_window);
    return *m_window;
}

System::Timer& Root::GetTimer()
{
    ASSERT(m_timer);
    return *m_timer;
}

System::InputManager& Root::GetInputManager()
{
    ASSERT(m_inputManager);
    return *m_inputManager;
}

System::ResourceManager& Root::GetResourceManager()
{
    ASSERT(m_resourceManager);
    return *m_resourceManager;
}

Graphics::RenderContext& Root::GetRenderContext()
{
    ASSERT(m_renderContext);
    return *m_renderContext;
}

Graphics::SpriteRenderer& Root::GetSpriteRenderer()
{
    ASSERT(m_spriteRenderer);
    return *m_spriteRenderer;
}

Renderer::StateRenderer& Root::GetStateRenderer()
{
    ASSERT(m_stateRenderer);
    return *m_stateRenderer;
}

Game::GameFramework& Engine::Root::GetGameFramework()
{
    ASSERT(m_gameFramework);
    return *m_gameFramework;
}

Editor::EditorSystem& Root::GetEditorSystem()
{
    ASSERT(m_editorSystem);
    return *m_editorSystem;
}
