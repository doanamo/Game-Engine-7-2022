/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#include "Precompiled.hpp"
#include "Engine.hpp"
#include <Build/Build.hpp>
#include <Reflection/Reflection.hpp>
#include <Core/Config.hpp>
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

namespace
{
    const char* CreateEngineError = "Failed to create engine! {}";
    const char* CreateServicesError = "Failed to create engine services! {}";
    const char* LoadDefaultResourcesError = "Failed to load default resources! {}";
}

Root::Root() = default;
Root::~Root() = default;

Root::CreateResult Root::Create(const Core::Config::VariableArray& configVars)
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
    Reflection::Initialize();

    auto instance = std::unique_ptr<Root>(new Root());

    if(auto failureResult = instance->CreateServices(configVars).AsFailure())
    {
        LOG_FATAL(CreateEngineError, "Could not create services.");
        return Common::Failure(failureResult.Unwrap());
    }

    if(auto failureResult = instance->LoadDefaultResources().AsFailure())
    {
        LOG_FATAL(CreateEngineError, "Could not load default resources.");
        return Common::Failure(failureResult.Unwrap());
    }

    if(auto config = instance->GetServices().Locate<Core::Config>())
    {
        float maxUpdateDelta = config->Get<float>(
            NAME_CONSTEXPR("engine.maxUpdateDelta"))
            .UnwrapOr(1.0f);

        if(maxUpdateDelta > 0.0f)
        {
            instance->m_maxUpdateDelta = maxUpdateDelta;
        }
        else
        {
            LOG_WARNING("Ignoring invalid \"engine.maxUpdateDelta={}\" "
                "config variable - value must be positive!", maxUpdateDelta);
            config->Set<float>(NAME_CONSTEXPR("engine.maxUpdateDelta"),
                instance->m_maxUpdateDelta, true);
        }
    }

    LOG_SUCCESS("Created engine instance.");
    return Common::Success(std::move(instance));
}

Common::Result<void, Root::CreateErrors> Root::CreateServices(
    const Core::Config::VariableArray& configVars)
{
    using ServicePtr = std::unique_ptr<Core::Service>;

    if(auto config = std::make_unique<Core::Config>())
    {
        config->Load(configVars);
        m_services.Attach(std::move(config));
    }
    else
    {
        LOG_ERROR(CreateServicesError, "Could not create config service.");
        return Common::Failure(CreateErrors::FailedServiceCreation);
    }

    // Information collection about engine's runtime performance.
    // Used to track and display simple measurements such as current frame rate.
    if(ServicePtr performanceMetrics = Core::PerformanceMetrics::Create()
        .UnwrapOr(nullptr))
    {
        m_services.Attach(std::move(performanceMetrics));
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
        m_services.Attach(std::move(platform));
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
        m_services.Attach(std::move(fileSystem));
    }
    else
    {
        LOG_ERROR(CreateServicesError, "Could not create file system service.");
        return Common::Failure(CreateErrors::FailedServiceCreation);
    }

    // Window management and back buffer presentation.
    // Collects and emits input events that can be listened to.
    if(ServicePtr window = std::make_unique<System::Window>())
    {
        m_services.Attach(std::move(window));
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
        m_services.Attach(std::move(timer));
    }
    else
    {
        LOG_ERROR(CreateServicesError, "Could not create timer service.");
        return Common::Failure(CreateErrors::FailedServiceCreation);
    }

    // Input event tracking and routing.
    // Tracks input states such as key presses collected from window.
    if(ServicePtr inputManager = System::InputManager::Create()
        .UnwrapOr(nullptr))
    {
        m_services.Attach(std::move(inputManager));
    }
    else
    {
        LOG_ERROR(CreateServicesError, "Could not create input manager service.");
        return Common::Failure(CreateErrors::FailedServiceCreation);
    }

    // Resource loading and reference counting.
    // Avoids subsequent loading of already loaded resources.
    if(ServicePtr resourceManager = System::ResourceManager::Create()
        .UnwrapOr(nullptr))
    {
        m_services.Attach(std::move(resourceManager));
    }
    else
    {
        LOG_ERROR(CreateServicesError, "Could not create resource manager service.");
        return Common::Failure(CreateErrors::FailedServiceCreation);
    }

    // Rendering context management.
    // State stack that minimizes changes submitted to graphics API.
    if(ServicePtr renderContext = Graphics::RenderContext::Create()
        .UnwrapOr(nullptr))
    {
        m_services.Attach(std::move(renderContext));
    }
    else
    {
        LOG_ERROR(CreateServicesError, "Could not create render context service.");
        return Common::Failure(CreateErrors::FailedServiceCreation);
    }

    // Fast textured quad drawing.
    // Sprites drawn using batching and instancing.
    if(ServicePtr spriteRenderer = Graphics::SpriteRenderer::Create()
        .UnwrapOr(nullptr))
    {
        m_services.Attach(std::move(spriteRenderer));
    }
    else
    {
        LOG_ERROR(CreateServicesError, "Could not create sprite renderer service.");
        return Common::Failure(CreateErrors::FailedServiceCreation);
    }

    // Game instance rendering.
    // Draws render components present in entities.
    if(ServicePtr stateRenderer = Renderer::GameRenderer::Create()
        .UnwrapOr(nullptr))
    {
        m_services.Attach(std::move(stateRenderer));
    }
    else
    {
        LOG_ERROR(CreateServicesError, "Could not create game renderer service.");
        return Common::Failure(CreateErrors::FailedServiceCreation);
    }

    // Game state management.
    // Decides how game state is updated, ticked and drawn.
    if(ServicePtr gameFramework = Game::GameFramework::Create()
        .UnwrapOr(nullptr))
    {
        m_services.Attach(std::move(gameFramework));
    }
    else
    {
        LOG_ERROR(CreateServicesError, "Could not create game framework service.");
        return Common::Failure(CreateErrors::FailedServiceCreation);
    }

    // Built-in editor interface.
    // Inspecting and modifying state of running engine and game.
    if(ServicePtr editorSystem = Editor::EditorSystem::Create()
        .UnwrapOr(nullptr))
    {
        m_services.Attach(std::move(editorSystem));
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
    if(gameFramework->ProcessGameState(timeDelta) ==
        Game::GameFramework::ProcessGameStateResults::TickedAndUpdated)
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
