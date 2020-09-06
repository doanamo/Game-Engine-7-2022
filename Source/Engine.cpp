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
#include <Graphics/Texture.hpp>
#include <Graphics/Sprite/SpriteRenderer.hpp>
#include <Renderer/GameRenderer.hpp>
#include <Game/GameFramework.hpp>
#include <Editor/EditorSystem.hpp>
using namespace Engine;

using ProcessGameStateResults = Game::GameFramework::ProcessGameStateResults;

namespace
{
    const char* CreateEngineError = "Failed to create engine! {}";
    const char* CreateServicesError = "Failed to create engine services! {}";
    const char* LoadDefaultResourcesError = "Failed to load default resources! {}";
}

Root::Root() = default;
Root::~Root() = default;

Root::CreateResult Root::Create(const CreateFromParams& params)
{
    // Create engine instance and return it if initialization succeeds.
    // First global systems are initialized for various debug facilities.
    // Then engine and its vital services are created so a game can be hosted.
    // At the end we load default resources such as placeholder texture.

    Debug::Initialize();
    Logger::Initialize();
    Build::Initialize();

    CHECK_ARGUMENT_OR_RETURN(params.maxUpdateDelta > 0.0f, Common::Failure(CreateErrors::InvalidArgument));

    auto instance = std::unique_ptr<Root>(new Root());
    instance->m_maxUpdateDelta = params.maxUpdateDelta;

    if(auto failureResult = instance->CreateServices().AsFailure())
    {
        LOG_FATAL(CreateEngineError, "Could not create services.");
        return Common::Failure(failureResult.Unwrap());
    }

    if(auto failureResult = instance->LoadDefaultResources().AsFailure())
    {
        LOG_FATAL(CreateEngineError, "Could not load default resources.");
        return Common::Failure(failureResult.Unwrap());
    }

    LOG_SUCCESS("Created engine instance.");
    return Common::Success(std::move(instance));
}

Common::Result<void, Root::CreateErrors> Root::CreateServices()
{
    // Information collection about engine's runtime performance.
    // Used to track and display simple measurements such as current frame rate.
    if(auto performanceMetrics = Core::PerformanceMetrics::Create().UnwrapOr(nullptr))
    {
        m_services.Provide(std::move(performanceMetrics));
    }
    else
    {
        LOG_ERROR(CreateServicesError, "Could not create performance metrics service.");
        return Common::Failure(CreateErrors::FailedServiceCreation);
    }

    // Base system for enabling windowing, timing and input collection.
    // Needs to be created first before the mentioned can be used.
    if(auto platform = System::Platform::Create().UnwrapOr(nullptr))
    {
        m_services.Provide(std::move(platform));
    }
    else
    {
        LOG_ERROR(CreateServicesError, "Could not create platform service.");
        return Common::Failure(CreateErrors::FailedServiceCreation);
    }

    // Path resolving with multiple mounted directories.
    if(auto fileSystem = System::FileSystem::Create().UnwrapOr(nullptr))
    {
        m_services.Provide(std::move(fileSystem));
    }
    else
    {
        LOG_ERROR(CreateServicesError, "Could not create file system service.");
        return Common::Failure(CreateErrors::FailedServiceCreation);
    }

    // Window management and back buffer presentation.
    // Collects and emits input events that can be listened to.
    System::Window::CreateFromParams windowParams;
    windowParams.title = "Game";
    windowParams.width = 1024;
    windowParams.height = 576;
    windowParams.vsync = true;
    windowParams.visible = true;

    if(auto window = System::Window::Create(windowParams).UnwrapOr(nullptr))
    {
        m_services.Provide(std::move(window));
    }
    else
    {
        LOG_ERROR(CreateServicesError, "Could not create window service.");
        return Common::Failure(CreateErrors::FailedServiceCreation);
    }

    // Main loop time tracking.
    // Used to calculate tick and update delta for each frame.
    if(auto timer = System::Timer::Create().UnwrapOr(nullptr))
    {
        m_services.Provide(std::move(timer));
    }
    else
    {
        LOG_ERROR(CreateServicesError, "Could not create timer service.");
        return Common::Failure(CreateErrors::FailedServiceCreation);
    }

    // Input event tracking and routing.
    // Tracks input states such as key presses collected from window.
    System::InputManager::CreateParams inputManagerParams;
    inputManagerParams.services = &m_services;

    if(auto inputManager = System::InputManager::Create(inputManagerParams).UnwrapOr(nullptr))
    {
        m_services.Provide(std::move(inputManager));
    }
    else
    {
        LOG_ERROR(CreateServicesError, "Could not create input manager service.");
        return Common::Failure(CreateErrors::FailedServiceCreation);
    }

    // Resource loading and reference counting.
    // Avoids subsequent loading of already loaded resources.
    System::ResourceManager::CreateFromParams resourceManagerParams;
    resourceManagerParams.services = &m_services;

    if(auto resourceManager = System::ResourceManager::Create(resourceManagerParams).UnwrapOr(nullptr))
    {
        m_services.Provide(std::move(resourceManager));
    }
    else
    {
        LOG_ERROR(CreateServicesError, "Could not create resource manager service.");
        return Common::Failure(CreateErrors::FailedServiceCreation);
    }

    // Rendering context management.
    // State stack that minimizes changes submitted to graphics API.
    Graphics::RenderContext::CreateParams renderContextParams;
    renderContextParams.services = &m_services;

    if(auto renderContext = Graphics::RenderContext::Create(renderContextParams).UnwrapOr(nullptr))
    {
        m_services.Provide(std::move(renderContext));
    }
    else
    {
        LOG_ERROR(CreateServicesError, "Could not create render context service.");
        return Common::Failure(CreateErrors::FailedServiceCreation);
    }

    // Fast textured quad drawing.
    // Sprites drawn using batching and instancing.
    Graphics::SpriteRenderer::CreateFromParams spriteRendererParams;
    spriteRendererParams.services = &m_services;
    spriteRendererParams.spriteBatchSize = 128;

    if(auto spriteRenderer = Graphics::SpriteRenderer::Create(spriteRendererParams).UnwrapOr(nullptr))
    {
        m_services.Provide(std::move(spriteRenderer));
    }
    else
    {
        LOG_ERROR(CreateServicesError, "Could not create sprite renderer service.");
        return Common::Failure(CreateErrors::FailedServiceCreation);
    }

    // Game instance rendering.
    // Draws render components present in entities.
    Renderer::GameRenderer::CreateFromParams stateRendererParams;
    stateRendererParams.services = &m_services;

    if(auto stateRenderer = Renderer::GameRenderer::Create(stateRendererParams).UnwrapOr(nullptr))
    {
        m_services.Provide(std::move(stateRenderer));
    }
    else
    {
        LOG_ERROR(CreateServicesError, "Could not create game renderer service.");
        return Common::Failure(CreateErrors::FailedServiceCreation);
    }

    // Game state management.
    // Decides how game state is updated, ticked and drawn.
    Game::GameFramework::CreateFromParams gameFrameworkParams;
    gameFrameworkParams.services = &m_services;

    if(auto gameFramework = Game::GameFramework::Create(gameFrameworkParams).UnwrapOr(nullptr))
    {
        m_services.Provide(std::move(gameFramework));
    }
    else
    {
        LOG_ERROR(CreateServicesError, "Could not create game framework service.");
        return Common::Failure(CreateErrors::FailedServiceCreation);
    }

    // Built-in editor interface.
    // Inspecting and modifying state of running engine and game.
    Editor::EditorSystem::CreateFromParams editorSystemParams;
    editorSystemParams.services = &m_services;

    if(auto editorSystem = Editor::EditorSystem::Create(editorSystemParams).UnwrapOr(nullptr))
    {
        m_services.Provide(std::move(editorSystem));
    }
    else
    {
        LOG_ERROR(CreateServicesError, "Could not create editor system service.");
        return Common::Failure(CreateErrors::FailedServiceCreation);
    }

    LOG_SUCCESS("Created engine services.");
    return Common::Success();
}

Common::Result<void, Root::CreateErrors> Root::LoadDefaultResources()
{
    System::FileSystem* fileSystem = m_services.GetFileSystem();
    System::ResourceManager* resourceManager = m_services.GetResourceManager();

    // Default texture placeholder for when requested texture is missing.
    // Texture is made to be easily spotted to indicate potential issues.
    std::unique_ptr<System::FileHandle> defaultTextureFileResult = fileSystem->OpenFile(
        "Data/Engine/Default/Texture.png", System::FileHandle::OpenFlags::Read).UnwrapOr(nullptr);

    if(defaultTextureFileResult != nullptr)
    {
        Graphics::Texture::LoadFromFile defaultTextureParams;
        defaultTextureParams.services = &m_services;

        if(auto defaultTextureResult = Graphics::Texture::Create(*defaultTextureFileResult, defaultTextureParams))
        {
            resourceManager->SetDefault<Graphics::Texture>(std::move(defaultTextureResult.Unwrap()));
        }
        else
        {
            LOG_ERROR(LoadDefaultResourcesError, "Could not load default texture resource.");
            return Common::Failure(CreateErrors::FailedResourceLoading);
        }
    }
    else
    {
        LOG_ERROR(LoadDefaultResourcesError, "Could not resolve default texture path.");
        return Common::Failure(CreateErrors::FailedResourceLoading);
    }
    
    LOG_SUCCESS("Loaded default engine resources.");
    return Common::Success();
}

void Root::ProcessFrame()
{
    // Single frame execution, running repeatedly in main loop.
    // Engine services are updated here each frame if needed.

    Logger::AdvanceFrameReference();

    Core::PerformanceMetrics* performanceMetrics = m_services.GetPerformanceMetrics();
    System::Timer* timer = m_services.GetTimer();
    System::Window* window = m_services.GetWindow();
    System::InputManager* inputManager = m_services.GetInputManager();
    System::ResourceManager* resourceManager = m_services.GetResourceManager();
    Game::GameFramework* gameFramework = m_services.GetGameFramework();
    Editor::EditorSystem* editorSystem = m_services.GetEditorSystem();

    float timeDelta = timer->Advance(m_maxUpdateDelta);

    performanceMetrics->MarkFrameStart();
    resourceManager->ReleaseUnused();
    window->ProcessEvents();

    editorSystem->BeginInterface(timeDelta);
    if(gameFramework->ProcessGameState(timeDelta) == ProcessGameStateResults::TickedAndUpdated)
    {
        inputManager->UpdateInputState(timeDelta);
    }
    editorSystem->EndInterface();

    window->Present();
    performanceMetrics->MarkFrameEnd();
}

Root::ErrorCode Root::Run()
{
    /*
        Initiates infinite main loop that exits only when application requests to be closed.
        Before main loop is run we have to set window context as current, then timer is reset
        before the first iteration to exclude time accumulated during initialization.
    */

    System::Timer* timer = m_services.GetTimer();
    System::Window* window = m_services.GetWindow();
    Game::GameFramework* gameFramework = m_services.GetGameFramework();

    window->MakeContextCurrent();
    timer->Reset();

    #ifndef __EMSCRIPTEN__
        while(true)
        {
            if(!window->ShouldClose())
            {
                LOG_INFO("Exiting main loop because window has been requested to close.");
                break;
            }

            if(!gameFramework->HasGameState())
            {
                LOG_INFO("Exiting main loop because there is no active game state.");
                break;
            }

            ProcessFrame();
        }
    #else
        auto mainLoopIteration = [](void* engine)
        {
            ASSERT(engine != nullptr);
            Root* root = static_cast<Root*>(engine);
            root->ProcessFrame();
        };

        emscripten_set_main_loop_arg(mainLoopIteration, this, 0, 1);
    #endif

    return ErrorCode(0);
}

const Core::ServiceStorage& Root::GetServices() const
{
    return m_services;
}
