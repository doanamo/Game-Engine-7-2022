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

Root::CreateResult Root::Create(const ConfigVariables& configVars)
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

Common::Result<void, Root::CreateErrors> Root::CreateServices(const ConfigVariables& configVars)
{
    // Create config system for service parametrization.
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

    // Create remaining engine services.
    Reflection::TypeIdentifier defaultEngineServiceTypes[] =
    {
        Reflection::GetIdentifier<Core::PerformanceMetrics>(),
        Reflection::GetIdentifier<System::Platform>(),
        Reflection::GetIdentifier<System::FileSystem>(),
        Reflection::GetIdentifier<System::Window>(),
        Reflection::GetIdentifier<System::InputManager>(),
        Reflection::GetIdentifier<System::ResourceManager>(),
        Reflection::GetIdentifier<System::Timer>(),
        Reflection::GetIdentifier<Graphics::RenderContext>(),
        Reflection::GetIdentifier<Graphics::SpriteRenderer>(),
        Reflection::GetIdentifier<Renderer::GameRenderer>(),
        Reflection::GetIdentifier<Game::GameFramework>(),
        Reflection::GetIdentifier<Editor::EditorSystem>(),
    };

    for(auto& engineServiceType : defaultEngineServiceTypes)
    {
        Core::ServiceStorage::ServicePtr engineService(
            Reflection::Construct<Core::Service>(engineServiceType));

        if(engineService != nullptr)
        {
            LOG_INFO("Created \"{}\" engine service.",
                Reflection::GetName(engineServiceType).GetString());

            if(!m_services.Attach(std::move(engineService)))
            {
                LOG_ERROR("Could not attach default engine service \"{}\"!",
                    Reflection::GetName(engineServiceType).GetString());
                return Common::Failure(CreateErrors::FailedServiceCreation);
            }
        }
        else
        {
            LOG_ERROR("Could not create default engine service \"{}\"!",
                Reflection::GetName(engineServiceType).GetString());
            return Common::Failure(CreateErrors::FailedServiceCreation);
        }
    }

    // Success!
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
