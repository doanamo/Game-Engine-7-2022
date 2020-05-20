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
    CHECK_ARGUMENT_OR_RETURN(params.maxTickDelta > 0.0f, Common::Failure(CreateErrors::InvalidArgument));

    // Initialize various debugging helpers.
    Debug::Initialize();

    // Initialize default logger output.
    Logger::Initialize();

    // Initialize information acquired from the build system.
    Build::Initialize();
    Build::PrintInfo();

    // Create instance.
    auto instance = std::unique_ptr<Root>(new Root());

    // Save maximum tick delta parameter.
    instance->m_maxTickDelta = params.maxTickDelta;

    // Create system platform context.
    // Enables use of platform specific systems such as window or input.
    auto platform = System::Platform::Create().UnwrapOr(nullptr);
    if(platform == nullptr)
    {
        LOG_ERROR("Could not create platform!");
        return Common::Failure(CreateErrors::FailedServiceCreation);
    }

    instance->m_services.Provide(std::move(platform));

    // Create file system.
    // Enables path resolving with multiple mounted directories.
    // Mount default directories (call order affects resolve order).
    auto fileSystem = System::FileSystem::Create().UnwrapOr(nullptr);
    if(fileSystem == nullptr)
    {
        LOG_ERROR("Could not create file system!");
        return Common::Failure(CreateErrors::FailedServiceCreation);
    }

    if(!Build::GetEngineDir().empty())
    {
        fileSystem->MountDirectory(Build::GetEngineDir());
    }

    if(!Build::GetGameDir().empty())
    {
        fileSystem->MountDirectory(Build::GetGameDir());
    }

    instance->m_services.Provide(std::move(fileSystem));

    // Create main window.
    // Collects input and presents swap chain content.
    // Window instance will create unique OpenGL context.
    System::Window::CreateFromParams windowParams;
    windowParams.title = "Game";
    windowParams.width = 1024;
    windowParams.height = 576;
    windowParams.vsync = true;
    windowParams.visible = true;

    auto window = System::Window::Create(windowParams).UnwrapOr(nullptr);
    if(window == nullptr)
    {
        LOG_ERROR("Could not create window!");
        return Common::Failure(CreateErrors::FailedServiceCreation);
    }

    instance->m_services.Provide(std::move(window));

    // Create timer.
    // Main timer that drives the main loop.
    auto timer = System::Timer::Create().UnwrapOr(nullptr);
    if(timer == nullptr)
    {
        LOG_ERROR("Could not create timer!");
        return Common::Failure(CreateErrors::FailedServiceCreation);
    }

    instance->m_services.Provide(std::move(timer));

    // Create input manager.
    // Collects and routes all events related to input.
    System::InputManager::CreateParams inputManagerParams;
    inputManagerParams.services = &instance->m_services;

    auto inputManager = System::InputManager::Create(inputManagerParams).UnwrapOr(nullptr);
    if(inputManager == nullptr)
    {
        LOG_ERROR("Could not create input manager!");
        return Common::Failure(CreateErrors::FailedServiceCreation);
    }

    instance->m_services.Provide(std::move(inputManager));

    // Create resource manager.
    // Helps avoid duplication of loaded resources.
    auto resourceManager = System::ResourceManager::Create().UnwrapOr(nullptr);
    if(resourceManager == nullptr)
    {
        LOG_ERROR("Could not create resource manager!");
        return Common::Failure(CreateErrors::FailedServiceCreation);
    }

    instance->m_services.Provide(std::move(resourceManager));

    // Create render context.
    // Manages rendering context created along with window.
    Graphics::RenderContext::CreateParams renderContextParams;
    renderContextParams.services = &instance->m_services;

    auto renderContext = Graphics::RenderContext::Create(renderContextParams).UnwrapOr(nullptr);
    if(renderContext == nullptr)
    {
        LOG_ERROR("Could not create render context!");
        return Common::Failure(CreateErrors::FailedServiceCreation);
    }

    instance->m_services.Provide(std::move(renderContext));

    // Create sprite renderer.
    // Rendering subsystem for drawing sprites.
    Graphics::SpriteRenderer::CreateFromParams spriteRendererParams;
    spriteRendererParams.services = &instance->m_services;
    spriteRendererParams.spriteBatchSize = 128;

    auto spriteRenderer = Graphics::SpriteRenderer::Create(spriteRendererParams).UnwrapOr(nullptr);
    if(spriteRenderer == nullptr)
    {
        LOG_ERROR("Could not create sprite renderer!");
        return Common::Failure(CreateErrors::FailedServiceCreation);
    }

    instance->m_services.Provide(std::move(spriteRenderer));

    // Create state renderer.
    // Draws game state represented by its render components.
    Renderer::StateRenderer::CreateFromParams stateRendererParams;
    stateRendererParams.services = &instance->m_services;

    auto stateRenderer = Renderer::StateRenderer::Create(stateRendererParams).UnwrapOr(nullptr);
    if(stateRenderer == nullptr)
    {
        LOG_ERROR("Could not create state renderer!");
        return Common::Failure(CreateErrors::FailedServiceCreation);
    }

    instance->m_services.Provide(std::move(stateRenderer));

    // Create game framework.
    // Wraps base functionality of shared game logic.
    Game::GameFramework::CreateFromParams gameFrameworkParams;
    gameFrameworkParams.services = &instance->m_services;

    auto gameFramework = Game::GameFramework::Create(gameFrameworkParams).UnwrapOr(nullptr);
    if(gameFramework == nullptr)
    {
        LOG_ERROR("Could not create game framework!");
        return Common::Failure(CreateErrors::FailedServiceCreation);
    }

    instance->m_services.Provide(std::move(gameFramework));

    // Create editor system.
    // Built in editor for creating and modifying content within running game.
    Editor::EditorSystem::CreateFromParams editorSystemParams;
    editorSystemParams.services = &instance->m_services;

    auto editorSystem = Editor::EditorSystem::Create(editorSystemParams).UnwrapOr(nullptr);
    if(editorSystem == nullptr)
    {
        LOG_ERROR("Could not create editor system!");
        return Common::Failure(CreateErrors::FailedServiceCreation);
    }

    instance->m_services.Provide(std::move(editorSystem));

    // Success!
    return Common::Success(std::move(instance));
}

int Root::Run()
{
    // Acquire engine services.
    System::Timer* timer = m_services.GetTimer();
    System::Window* window = m_services.GetWindow();

    // Ensure that window context is current.
    window->MakeContextCurrent();

    // Reset time that has accumulated during initialization.
    timer->Reset();

    // Define main loop iteration.
    auto mainLoopIteration = [](void* engine)
    {
        // Retrieve engine root.
        ASSERT(engine != nullptr);
        Root& root = *static_cast<Root*>(engine);

        // Acquire engine services.
        const Core::ServiceStorage& services = root.GetServices();

        System::Timer* timer = services.GetTimer();
        System::Window* window = services.GetWindow();
        System::InputManager* inputManager = services.GetInputManager();
        System::ResourceManager* resourceManager = services.GetResourceManager();
        Game::GameFramework* gameFramework = services.GetGameFramework();
        Editor::EditorSystem* editorSystem = services.GetEditorSystem();

        // Advance logger's frame of reference.
        Logger::AdvanceFrameReference();

        // Release unused resources.
        resourceManager->ReleaseUnused();

        // Tick timer to calculate delta.
        float timeDelta = timer->Tick(root.m_maxTickDelta);

        // Process window events.
        window->ProcessEvents();

        // Update editor system.
        editorSystem->Update(timeDelta);

        // Update game state.
        if(gameFramework->Update())
        {
            // Prepare input manager for incoming events.
            inputManager->AdvanceState(timeDelta);
        }

        // Draw game state.
        gameFramework->Draw();

        // Draw editor system.
        editorSystem->Draw();

        // Present window content.
        window->Present();
    };

    // Run main loop.
    #ifndef __EMSCRIPTEN__
        while(window->IsOpen())
        {
            mainLoopIteration(this);
        }
    #else
        emscripten_set_main_loop_arg(mainLoopIteration, this, 0, 1);
    #endif

    // Return error code.
    return 0;
}

const Core::ServiceStorage& Root::GetServices() const
{
    return m_services;
}
