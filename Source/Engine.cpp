/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#include "Precompiled.hpp"
#include "Engine.hpp"
#include <Build/Build.hpp>
#include <Core/PerformanceMetrics.hpp>
#include <System/Platform.hpp>
#include <System/Timer.hpp>
#include <System/FileSystem/FileSystem.hpp>
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

#include <Common/ReflectionGenerated.hpp>
#include <Core/ReflectionGenerated.hpp>
#include <System/ReflectionGenerated.hpp>
#include <Graphics/ReflectionGenerated.hpp>
#include <Game/ReflectionGenerated.hpp>
#include <Renderer/ReflectionGenerated.hpp>
#include <Editor/ReflectionGenerated.hpp>

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
    /*
        Create engine instance and return it if initialization succeeds.
        First global systems are initialized for various debug facilities.
        Then engine and its services are created so game state can be hosted.
        At the end we load default resources such as placeholder texture.
    */

    Debug::Initialize();
    Logger::Initialize();
    Build::Initialize();

    Reflection::Generated::RegisterModuleCommon();
    Reflection::Generated::RegisterModuleCore();
    Reflection::Generated::RegisterModuleSystem();
    Reflection::Generated::RegisterModuleGraphics();
    Reflection::Generated::RegisterModuleGame();
    Reflection::Generated::RegisterModuleRenderer();
    Reflection::Generated::RegisterModuleEditor();

    CHECK_ARGUMENT_OR_RETURN(params.maxUpdateDelta > 0.0f,
        Common::Failure(CreateErrors::InvalidArgument));

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
    using ServicePtr = std::unique_ptr<Core::Service>;

    // Information collection about engine's runtime performance.
    // Used to track and display simple measurements such as current frame rate.
    if(ServicePtr performanceMetrics = Core::PerformanceMetrics::Create()
        .UnwrapOr(nullptr))
    {
        m_services.Provide(performanceMetrics);
    }
    else
    {
        LOG_ERROR(CreateServicesError, "Could not create performance metrics service.");
        return Common::Failure(CreateErrors::FailedServiceCreation);
    }

    // Base system for enabling windowing, timing and input collection.
    // Needs to be created first before the mentioned can be used.
    if(ServicePtr platform = System::Platform::Create()
        .UnwrapOr(nullptr))
    {
        m_services.Provide(platform);
    }
    else
    {
        LOG_ERROR(CreateServicesError, "Could not create platform service.");
        return Common::Failure(CreateErrors::FailedServiceCreation);
    }

    // Path resolving with multiple mounted directories.
    if(ServicePtr fileSystem = System::FileSystem::Create()
        .UnwrapOr(nullptr))
    {
        m_services.Provide(fileSystem);
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
    windowParams.vsync = false;
    windowParams.visible = true;

    if(ServicePtr window = System::Window::Create(windowParams)
        .UnwrapOr(nullptr))
    {
        m_services.Provide(window);
    }
    else
    {
        LOG_ERROR(CreateServicesError, "Could not create window service.");
        return Common::Failure(CreateErrors::FailedServiceCreation);
    }

    // Main loop time tracking.
    // Used to calculate tick and update delta for each frame.
    if(ServicePtr timer = System::Timer::Create()
        .UnwrapOr(nullptr))
    {
        m_services.Provide(timer);
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

    if(ServicePtr inputManager = System::InputManager::Create(inputManagerParams)
        .UnwrapOr(nullptr))
    {
        m_services.Provide(inputManager);
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

    if(ServicePtr resourceManager = System::ResourceManager::Create(resourceManagerParams)
        .UnwrapOr(nullptr))
    {
        m_services.Provide(resourceManager);
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

    if(ServicePtr renderContext = Graphics::RenderContext::Create(renderContextParams)
        .UnwrapOr(nullptr))
    {
        m_services.Provide(renderContext);
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

    if(ServicePtr spriteRenderer = Graphics::SpriteRenderer::Create(spriteRendererParams)
        .UnwrapOr(nullptr))
    {
        m_services.Provide(spriteRenderer);
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

    if(ServicePtr stateRenderer = Renderer::GameRenderer::Create(stateRendererParams)
        .UnwrapOr(nullptr))
    {
        m_services.Provide(stateRenderer);
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

    if(ServicePtr gameFramework = Game::GameFramework::Create(gameFrameworkParams)
        .UnwrapOr(nullptr))
    {
        m_services.Provide(gameFramework);
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

    if(ServicePtr editorSystem = Editor::EditorSystem::Create(editorSystemParams)
        .UnwrapOr(nullptr))
    {
        m_services.Provide(editorSystem);
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
    auto* fileSystem = m_services.Locate<System::FileSystem>();
    auto* resourceManager = m_services.Locate<System::ResourceManager>();

    // Default texture placeholder for when requested texture is missing.
    // Texture is made to be easily spotted to indicate potential issues.
    const std::unique_ptr<System::FileHandle> defaultTextureFileResult = fileSystem->OpenFile(
        "Data/Engine/Default/Texture.png", System::FileHandle::OpenFlags::Read)
        .UnwrapOr(nullptr);

    if(defaultTextureFileResult != nullptr)
    {
        Graphics::Texture::LoadFromFile defaultTextureParams;
        defaultTextureParams.services = &m_services;

        if(auto defaultTextureResult = Graphics::Texture::Create(
            *defaultTextureFileResult, defaultTextureParams))
        {
            resourceManager->SetDefault<Graphics::Texture>(defaultTextureResult.Unwrap());
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
    /*
        Single frame execution, running repeatedly in main loop.
        Engine services are updated here each frame if needed.
    */

    using ProcessGameStateResults = Game::GameFramework::ProcessGameStateResults;

    Logger::AdvanceFrameReference();

    auto* performanceMetrics = m_services.Locate<Core::PerformanceMetrics>();
    auto* timer = m_services.Locate<System::Timer>();
    auto* window = m_services.Locate<System::Window>();
    auto* inputManager = m_services.Locate<System::InputManager>();
    auto* resourceManager = m_services.Locate<System::ResourceManager>();
    auto* gameFramework = m_services.Locate<Game::GameFramework>();
    auto* editorSystem = m_services.Locate<Editor::EditorSystem>();

    const float timeDelta = timer->Advance(m_maxUpdateDelta);

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
        Initiates infinite main loop that exits only when application requests
        to be closed. Before main loop is run we have to set window context as
        current, then timer is reset on the first iteration to exclude time
        accumulated during initialization.
    */

    auto* timer = m_services.Locate<System::Timer>();
    auto* window = m_services.Locate<System::Window>();
    auto* gameFramework = m_services.Locate<Game::GameFramework>();

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
