/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#include "Precompiled.hpp"
#include "Engine.hpp"
#include <Build/Build.hpp>
#include <Core/PerformanceMetrics.hpp>
#include <System/Platform.hpp>
#include <System/Timer.hpp>
#include <System/FileSystem.hpp>
#include <System/ResourceManager.hpp>
#include <System/InputManager.hpp>
#include <System/Window.hpp>
#include <Graphics/RenderContext.hpp>
#include <Graphics/Sprite/SpriteRenderer.hpp>
#include <Renderer/GameRenderer.hpp>
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

    // Measure creation time.
    const auto creationStartTime = std::chrono::steady_clock::now();

    // Initialize static systems.
    Debug::Initialize();
    Logger::Initialize();
    Build::Initialize();

    // Create instance.
    auto instance = std::unique_ptr<Root>(new Root());

    // Save maximum tick delta parameter.
    instance->m_maxTickDelta = params.maxTickDelta;

    // Create performance metrics.
    // Collects information about engine's runtime performance.
    // Must be initialized after platform to have access to the timer.
    if(auto performanceMetrics = Core::PerformanceMetrics::Create().UnwrapOr(nullptr))
    {
        instance->m_services.Provide(std::move(performanceMetrics));
    }
    else
    {
        LOG_ERROR("Could not create performance metrics!");
        return Common::Failure(CreateErrors::FailedServiceCreation);
    }

    // Create system platform context.
    // Enables use of platform specific systems such as window or input.
    if(auto platform = System::Platform::Create().UnwrapOr(nullptr))
    {
        instance->m_services.Provide(std::move(platform));
    }
    else
    {
        LOG_ERROR("Could not create platform!");
        return Common::Failure(CreateErrors::FailedServiceCreation);
    }

    // Create file system.
    // Enables path resolving with multiple mounted directories.
    // Mount default directories (call order affects resolve order).
    if(auto fileSystem = System::FileSystem::Create().UnwrapOr(nullptr))
    {
        fileSystem->MountDirectory("./");

        if(!Build::GetEngineDir().empty())
        {
            fileSystem->MountDirectory(Build::GetEngineDir());
        }

        if(!Build::GetGameDir().empty())
        {
            fileSystem->MountDirectory(Build::GetGameDir());
        }

        instance->m_services.Provide(std::move(fileSystem));
    }
    else
    {
        LOG_ERROR("Could not create file system!");
        return Common::Failure(CreateErrors::FailedServiceCreation);
    }

    // Create main window.
    // Collects input and presents swap chain content.
    // Window instance will create unique OpenGL context.
    System::Window::CreateFromParams windowParams;
    windowParams.title = "Game";
    windowParams.width = 1024;
    windowParams.height = 576;
    windowParams.vsync = true;
    windowParams.visible = true;

    if(auto window = System::Window::Create(windowParams).UnwrapOr(nullptr))
    {
        instance->m_services.Provide(std::move(window));
    }
    else
    {
        LOG_ERROR("Could not create window!");
        return Common::Failure(CreateErrors::FailedServiceCreation);
    }

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

    if(auto inputManager = System::InputManager::Create(inputManagerParams).UnwrapOr(nullptr))
    {
        instance->m_services.Provide(std::move(inputManager));
    }
    else
    {
        LOG_ERROR("Could not create input manager!");
        return Common::Failure(CreateErrors::FailedServiceCreation);
    }

    // Create resource manager.
    // Helps avoid duplication of loaded resources.
    System::ResourceManager::CreateFromParams resourceManagerParams;
    resourceManagerParams.services = &instance->m_services;

    if(auto resourceManager = System::ResourceManager::Create(resourceManagerParams).UnwrapOr(nullptr))
    {
        instance->m_services.Provide(std::move(resourceManager));
    }
    else
    {
        LOG_ERROR("Could not create resource manager!");
        return Common::Failure(CreateErrors::FailedServiceCreation);
    }

    // Create render context.
    // Manages rendering context created along with window.
    Graphics::RenderContext::CreateParams renderContextParams;
    renderContextParams.services = &instance->m_services;

    if(auto renderContext = Graphics::RenderContext::Create(renderContextParams).UnwrapOr(nullptr))
    {
        instance->m_services.Provide(std::move(renderContext));
    }
    else
    {
        LOG_ERROR("Could not create render context!");
        return Common::Failure(CreateErrors::FailedServiceCreation);
    }

    // Create sprite renderer.
    // Rendering subsystem for drawing sprites.
    Graphics::SpriteRenderer::CreateFromParams spriteRendererParams;
    spriteRendererParams.services = &instance->m_services;
    spriteRendererParams.spriteBatchSize = 128;

    if(auto spriteRenderer = Graphics::SpriteRenderer::Create(spriteRendererParams).UnwrapOr(nullptr))
    {
        instance->m_services.Provide(std::move(spriteRenderer));
    }
    else
    {
        LOG_ERROR("Could not create sprite renderer!");
        return Common::Failure(CreateErrors::FailedServiceCreation);
    }

    // Create game renderer.
    // Draws game instance represented by its render components.
    Renderer::GameRenderer::CreateFromParams stateRendererParams;
    stateRendererParams.services = &instance->m_services;

    if(auto stateRenderer = Renderer::GameRenderer::Create(stateRendererParams).UnwrapOr(nullptr))
    {
        instance->m_services.Provide(std::move(stateRenderer));
    }
    else
    {
        LOG_ERROR("Could not create game renderer!");
        return Common::Failure(CreateErrors::FailedServiceCreation);
    }

    // Create game framework.
    // Wraps base functionality of shared game logic.
    Game::GameFramework::CreateFromParams gameFrameworkParams;
    gameFrameworkParams.services = &instance->m_services;

    if(auto gameFramework = Game::GameFramework::Create(gameFrameworkParams).UnwrapOr(nullptr))
    {
        instance->m_services.Provide(std::move(gameFramework));
    }
    else
    {
        LOG_ERROR("Could not create game framework!");
        return Common::Failure(CreateErrors::FailedServiceCreation);
    }

    // Create editor system.
    // Built in editor for creating and modifying content within running game.
    Editor::EditorSystem::CreateFromParams editorSystemParams;
    editorSystemParams.services = &instance->m_services;

    if(auto editorSystem = Editor::EditorSystem::Create(editorSystemParams).UnwrapOr(nullptr))
    {
        instance->m_services.Provide(std::move(editorSystem));
    }
    else
    {
        LOG_ERROR("Could not create editor system!");
        return Common::Failure(CreateErrors::FailedServiceCreation);
    }

    // Load default engine resources.
    if(!instance->LoadDefaultResources())
    {
        LOG_ERROR("Could not load default resources!");
        return Common::Failure(CreateErrors::FailedResourceLoading);
    }

    // Print engine creation time.
    const auto creationEndTime = std::chrono::steady_clock::now();
    std::chrono::duration<float> creationTime = creationEndTime - creationStartTime;
    LOG_INFO("Engine creation took {:.2f} seconds.", creationTime.count());

    // Success!
    return Common::Success(std::move(instance));
}

bool Root::LoadDefaultResources()
{
    LOG_INFO("Loading default resources...");
    LOG_SCOPED_INDENT();

    // Acquire resource manager.
    System::FileSystem* fileSystem = m_services.GetFileSystem();
    System::ResourceManager* resourceManager = m_services.GetResourceManager();

    // Load default texture.
    auto texturePathResult = fileSystem->ResolvePath("Data/Engine/Default/Texture.png");
    if(!texturePathResult)
        return false;

    Graphics::Texture::LoadFromFile defaultTextureParams;
    defaultTextureParams.services = &m_services;

    auto defaultTextureResult = Graphics::Texture::Create(texturePathResult.Unwrap(), defaultTextureParams);
    if(!defaultTextureResult)
        return false;

    resourceManager->SetDefault<Graphics::Texture>(std::move(defaultTextureResult.Unwrap()));

    // Success!
    return true;
}

int Root::Run()
{
    // Acquire engine services.
    System::Timer* timer = m_services.GetTimer();
    System::Window* window = m_services.GetWindow();
    Game::GameFramework* gameFramework = m_services.GetGameFramework();

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

        Core::PerformanceMetrics* performanceMetrics = services.GetPerformanceMetrics();
        System::Timer* timer = services.GetTimer();
        System::Window* window = services.GetWindow();
        System::InputManager* inputManager = services.GetInputManager();
        System::ResourceManager* resourceManager = services.GetResourceManager();
        Game::GameFramework* gameFramework = services.GetGameFramework();
        Editor::EditorSystem* editorSystem = services.GetEditorSystem();

        // Mark frame start.
        performanceMetrics->StartFrame();

        // Advance logger's frame of reference.
        Logger::AdvanceFrameReference();

        // Release unused resources.
        resourceManager->ReleaseUnused();

        // Advance timer to calculate delta.
        float timeDelta = timer->Advance(root.m_maxTickDelta);

        // Process window events.
        window->ProcessEvents();

        // Update editor system.
        editorSystem->Update(timeDelta);

        // Trigger game update, trick and draw logic.
        // Update returns true if tick was processed.
        if(gameFramework->ProcessGameState(timeDelta))
        {
            // Prepare input manager for incoming events.
            inputManager->UpdateInputState(timeDelta);
        }

        // Draw editor system.
        editorSystem->Draw();

        // Present window content.
        window->Present();

        // Mark frame end.
        performanceMetrics->EndFrame();
    };

    // Run main loop.
    #ifndef __EMSCRIPTEN__
        while(true)
        {
            // Check exit conditions.
            if(!window->ShouldClose())
            {
                LOG_INFO("Breaking out of main loop because window has been requested to close.");
                break;
            }

            if(!gameFramework->HasGameState())
            {
                LOG_INFO("Breaking out of main loop because there is no active game state.");
                break;
            }

            // Run main loop iteration.
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
