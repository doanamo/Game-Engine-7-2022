/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#include "Precompiled.hpp"
#include "Engine.hpp"
#include <Build/Build.hpp>
#include <Reflection/Reflection.hpp>
#include <Core/ConfigSystem.hpp>
#include <Core/EngineMetrics.hpp>
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
    const char* CreateSystemsError = "Failed to create engine systems! {}";
    const char* LoadDefaultResourcesError = "Failed to load default resources! {}";
}

Root::Root() = default;
Root::~Root() = default;

Root::CreateResult Root::Create(const ConfigVariables& configVars)
{
    /*
        Create engine instance and return it if initialization succeeds.
        First global systems are initialized for various debug facilities.
        Then engine and its systems are created so game state can be hosted.
        At the end we load default resources such as placeholder texture.
    */

    Debug::Initialize();
    Logger::Initialize();
    Build::Initialize();
    Reflection::Initialize();

    auto engine = std::unique_ptr<Root>(new Root());

    if(auto failureResult = engine->CreateEngineSystems(configVars).AsFailure())
    {
        LOG_FATAL(CreateEngineError, "Could not create engine systems.");
        return Common::Failure(failureResult.Unwrap());
    }

    if(auto failureResult = engine->LoadDefaultResources().AsFailure())
    {
        LOG_FATAL(CreateEngineError, "Could not load default resources.");
        return Common::Failure(failureResult.Unwrap());
    }

    if(auto config = engine->GetSystems().Locate<Core::ConfigSystem>())
    {
        float maxUpdateDelta = config->Get<float>(
            NAME_CONSTEXPR("engine.maxUpdateDelta"))
            .UnwrapOr(1.0f);

        if(maxUpdateDelta > 0.0f)
        {
            engine->m_maxUpdateDelta = maxUpdateDelta;
        }
        else
        {
            LOG_WARNING("Ignoring invalid \"engine.maxUpdateDelta={}\" "
                "config variable - value must be positive!", maxUpdateDelta);
            config->Set<float>(NAME_CONSTEXPR("engine.maxUpdateDelta"),
                engine->m_maxUpdateDelta, true);
        }
    }

    LOG_SUCCESS("Created engine instance.");
    return Common::Success(std::move(engine));
}

Common::Result<void, Root::CreateErrors> Root::CreateEngineSystems(const ConfigVariables& configVars)
{
    // Create config system for engine parametrization.
    if(auto config = std::make_unique<Core::ConfigSystem>())
    {
        config->Load(configVars);
        m_engineSystems.Attach(std::move(config));
    }
    else
    {
        LOG_ERROR(CreateSystemsError, "Could not create config system.");
        return Common::Failure(CreateErrors::FailedSystemCreation);
    }

    // Create remaining engine systems.
    const std::vector<Reflection::TypeIdentifier> defaultEngineSystemTypes =
    {
        Reflection::GetIdentifier<Core::EngineMetrics>(),
        Reflection::GetIdentifier<System::Platform>(),
        Reflection::GetIdentifier<System::FileSystem>(),
        Reflection::GetIdentifier<System::Window>(),
        Reflection::GetIdentifier<System::InputManager>(),
        Reflection::GetIdentifier<System::ResourceManager>(),
        Reflection::GetIdentifier<System::Timer>(),
        Reflection::GetIdentifier<Game::GameFramework>(),
        Reflection::GetIdentifier<Graphics::RenderContext>(),
        Reflection::GetIdentifier<Graphics::SpriteRenderer>(),
        Reflection::GetIdentifier<Renderer::GameRenderer>(),
        Reflection::GetIdentifier<Editor::EditorSystem>(),
    };

    if(!m_engineSystems.CreateFromTypes(defaultEngineSystemTypes))
    {
        LOG_ERROR(CreateSystemsError, "Could not populate system storage.");
        return Common::Failure(CreateErrors::FailedSystemCreation);
    }

    // Success!
    LOG_SUCCESS("Created engine systems.");
    return Common::Success();
}

Common::Result<void, Root::CreateErrors> Root::LoadDefaultResources()
{
    auto* fileSystem = m_engineSystems.Locate<System::FileSystem>();
    auto* resourceManager = m_engineSystems.Locate<System::ResourceManager>();

    // Default texture placeholder for when requested texture is missing.
    // Texture is made to be easily spotted to indicate potential issues.
    const std::unique_ptr<System::FileHandle> defaultTextureFileResult = fileSystem->OpenFile(
        "Data/Engine/Default/Texture.png", System::FileHandle::OpenFlags::Read)
        .UnwrapOr(nullptr);

    if(defaultTextureFileResult != nullptr)
    {
        Graphics::Texture::LoadFromFile defaultTextureParams;
        defaultTextureParams.engineSystems = &m_engineSystems;

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
        Engine systems are updated here each frame if needed.
    */

    Logger::AdvanceFrameReference();

    auto* metrics = m_engineSystems.Locate<Core::EngineMetrics>();
    auto* timer = m_engineSystems.Locate<System::Timer>();
    auto* window = m_engineSystems.Locate<System::Window>();
    auto* inputManager = m_engineSystems.Locate<System::InputManager>();
    auto* resourceManager = m_engineSystems.Locate<System::ResourceManager>();
    auto* gameFramework = m_engineSystems.Locate<Game::GameFramework>();
    auto* editorSystem = m_engineSystems.Locate<Editor::EditorSystem>();

    const float timeDelta = timer->Advance(m_maxUpdateDelta);

    metrics->MarkFrameStart();
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
    metrics->MarkFrameEnd();
}

Root::ErrorCode Root::Run()
{
    /*
        Initiates infinite main loop that exits only when application requests
        to be closed. Before main loop is run we have to set window context as
        current, then timer is reset on the first iteration to exclude time
        accumulated during initialization.
    */

    auto* timer = m_engineSystems.Locate<System::Timer>();
    auto* window = m_engineSystems.Locate<System::Window>();
    auto* gameFramework = m_engineSystems.Locate<Game::GameFramework>();

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
