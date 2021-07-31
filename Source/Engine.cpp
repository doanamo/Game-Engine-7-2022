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
    const char* LogCreateEngineFailed = "Failed to create engine! {}";
    const char* LogCreateSystemsFailed = "Failed to create engine systems! {}";
    const char* LogLoadDefaultResourcesFailed = "Failed to load default resources! {}";
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

    LOG_PROFILE_SCOPE("Create engine");

    // Initialize static systems.
    Debug::Initialize();
    Logger::Initialize();
    Build::Initialize();
    Reflection::Initialize();

    // Create engine instance.
    auto engine = std::unique_ptr<Root>(new Root());

    if(auto failureResult = engine->CreateEngineSystems(configVars).AsFailure())
    {
        LOG_FATAL(LogCreateEngineFailed, "Could not create engine systems.");
        return Common::Failure(failureResult.Unwrap());
    }

    if(auto failureResult = engine->LoadDefaultResources().AsFailure())
    {
        LOG_FATAL(LogCreateEngineFailed, "Could not load default resources.");
        return Common::Failure(failureResult.Unwrap());
    }

    return Common::Success(std::move(engine));
}

Common::Result<void, Root::CreateErrors> Root::CreateEngineSystems(const ConfigVariables& configVars)
{
    LOG_PROFILE_SCOPE("Create engine systems");

    // Create config system for engine parametrization.
    if(auto config = std::make_unique<Core::ConfigSystem>())
    {
        config->Load(configVars);
        m_engineSystems.Attach(std::move(config));
    }
    else
    {
        LOG_ERROR(LogCreateSystemsFailed, "Could not create config system.");
        return Common::Failure(CreateErrors::FailedSystemCreation);
    }

    // Create remaining engine systems.
    const std::vector<Reflection::TypeIdentifier> defaultEngineSystemTypes =
    {
        Reflection::GetIdentifier<Core::EngineMetrics>(),
        Reflection::GetIdentifier<System::Platform>(),
        Reflection::GetIdentifier<System::FileSystem>(),
        Reflection::GetIdentifier<System::Timer>(),
        Reflection::GetIdentifier<System::Window>(),
        Reflection::GetIdentifier<System::InputManager>(),
        Reflection::GetIdentifier<System::ResourceManager>(),
        Reflection::GetIdentifier<Game::GameFramework>(),
        Reflection::GetIdentifier<Graphics::RenderContext>(),
        Reflection::GetIdentifier<Graphics::SpriteRenderer>(),
        Reflection::GetIdentifier<Renderer::GameRenderer>(),
        Reflection::GetIdentifier<Editor::EditorSystem>(),
    };

    if(!m_engineSystems.CreateFromTypes(defaultEngineSystemTypes))
    {
        LOG_ERROR(LogCreateSystemsFailed, "Could not populate system storage.");
        return Common::Failure(CreateErrors::FailedSystemCreation);
    }

    if(!m_engineSystems.Finalize())
    {
        LOG_ERROR(LogCreateSystemsFailed, "Could not finalize system storage.");
        return Common::Failure(CreateErrors::FailedSystemCreation);
    }

    return Common::Success();
}

Common::Result<void, Root::CreateErrors> Root::LoadDefaultResources()
{
    LOG_PROFILE_SCOPE("Load default engine resources");

    // Locate systems needed to load resources.
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
            LOG_ERROR(LogLoadDefaultResourcesFailed, "Could not load default texture resource.");
            return Common::Failure(CreateErrors::FailedResourceLoading);
        }
    }
    else
    {
        LOG_ERROR(LogLoadDefaultResourcesFailed, "Could not resolve default texture path.");
        return Common::Failure(CreateErrors::FailedResourceLoading);
    }

    return Common::Success();
}

void Root::ProcessFrame()
{
    // Begin processing frame.
    Logger::AdvanceFrameReference();

    m_engineSystems.ForEach([](Core::EngineSystem& engineSystem)
    {
        engineSystem.OnBeginFrame();
        return true;
    });

    // Perform frame processing.
    m_engineSystems.ForEach([](Core::EngineSystem& engineSystem)
    {
        engineSystem.OnProcessFrame();
        return true;
    });

    // End processing frame.
    m_engineSystems.ForEachReverse([](Core::EngineSystem& engineSystem)
    {
        engineSystem.OnEndFrame();
        return true;
    });
}

Root::ErrorCode Root::Run()
{
    /*
        Initiates main loop that exits only when application requests to be closed. Before main
        loop is run we have to set window context as current, then timer is reset on the first
        iteration to exclude time accumulated during initialization.
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
