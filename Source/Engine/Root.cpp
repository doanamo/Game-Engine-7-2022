/*
    Copyright (c) 2018 Piotr Doan. All rights reserved.
*/

#include "Precompiled.hpp"
#include "System/Platform.hpp"
#include "System/FileSystem.hpp"
#include "System/Window.hpp"
#include "System/Timer.hpp"
#include "System/InputState.hpp"
#include "System/ResourceManager.hpp"
#include "Graphics/RenderContext.hpp"
#include "Graphics/Sprite/SpriteRenderer.hpp"
#include "Renderer/StateRenderer.hpp"
#include "Editor/EditorSystem.hpp"
#include "Engine/Root.hpp"
using namespace Engine;

Root::Root() :
    m_initialized(false)
{
}

Root::~Root()
{
}

Root::Root(Root&& other) :
    Root()
{
    *this = std::move(other);
}

Root& Root::operator=(Root&& other)
{
    std::swap(m_platform, other.m_platform);
    std::swap(m_fileSystem, other.m_fileSystem);
    std::swap(m_window, other.m_window);
    std::swap(m_timer, other.m_timer);
    std::swap(m_inputState, other.m_inputState);
    std::swap(m_resourceManager, other.m_resourceManager);
    std::swap(m_renderContext, other.m_renderContext);
    std::swap(m_spriteRenderer, other.m_spriteRenderer);
    std::swap(m_stateRenderer, other.m_stateRenderer);
    std::swap(m_editorSystem, other.m_editorSystem);
    std::swap(m_initialized, other.m_initialized);

    return *this;
}

bool Root::Initialize()
{
    // Verify that engine is not currently initialized.
    VERIFY(!m_initialized, "Engine instance has already been initialized!");

    // Initialize various debugging helpers.
    Debug::Initialize();

    // Initialize the default logger output.
    Logger::Initialize();

    // Initialize information acquired from the build system.
    Build::Initialize();

    // After low level system have been initialized, begin initializing other systems.
    LOG() << "Initializing engine..." << LOG_INDENT();

    // Reset class instance on initialization failure.
    SCOPE_GUARD_IF(!m_initialized, *this = Root());

    // Initialize the system platform context.
    // This will allow us to create and use platform systems such as window or input.
    m_platform = std::make_unique<System::Platform>();
    if(!m_platform->Initialize())
    {
        LOG_ERROR() << "Could not initialize platform!";
        return false;
    }

    // Initialize the file system.
    m_fileSystem = std::make_unique<System::FileSystem>();
    if(!m_fileSystem->Initialize())
    {
        LOG_ERROR() << "Could not initialize file system!";
        return false;
    }

    // Mount file system directories (order affects the resolve order).
    if(!Build::GetEngineDir().empty())
    {
        m_fileSystem->MountDirectory(Build::GetEngineDir());
    }

    if(!Build::GetGameDir().empty())
    {
        m_fileSystem->MountDirectory(Build::GetGameDir());
    }

    // Initialize the main window.
    // We will be collecting input and then drawing into this window.
    // Window instance will create an unique OpenGL context for us.
    System::WindowInfo windowInfo;
    windowInfo.title = "Game";
    windowInfo.width = 1024;
    windowInfo.height = 576;
    windowInfo.vsync = true;
    windowInfo.visible = true;

    m_window = std::make_unique<System::Window>();
    if(!m_window->Initialize(windowInfo))
    {
        LOG_ERROR() << "Could not initialize window!";
        return false;
    }

    // Initialize the main timer.
    // There can be many timers but this one will be used to calculate frame time.
    m_timer = std::make_unique<System::Timer>();
    if(!m_timer->Initialize())
    {
        LOG_ERROR() << "Could not initialize timer!";
        return false;
    }

    // Initialize the input state.
    // Collects and caches input state that can be later pooled.
    m_inputState = std::make_unique<System::InputState>();
    if(!m_inputState->Initialize(m_window.get()))
    {
        LOG_ERROR() << "Could not initialize input state!";
        return false;
    }

    // Initialize the resource manager.
    // Resource manager will help avoid duplication of resources.
    m_resourceManager = std::make_unique<System::ResourceManager>();
    if(!m_resourceManager->Initialize())
    {
        LOG_ERROR() << "Could not initialize resource manager!";
        return false;
    }

    // Initialize the graphics context.
    // Manages the rendering context created along with the window.
    m_renderContext = std::make_unique<Graphics::RenderContext>();
    if(!m_renderContext->Initialize(m_window.get()))
    {
        LOG_ERROR() << "Could not initialize graphics context!";
        return false;
    }

    // Initialize the sprite renderer.
    // Rendering subsystem for drawing sprites.
    m_spriteRenderer = std::make_unique<Graphics::SpriteRenderer>();
    if(!m_spriteRenderer->Initialize(this, 128))
    {
        LOG_ERROR() << "Could not initialize sprite renderer!";
        return false;
    }

    // Initialize the state renderer.
    // Renders a game state described in its components.
    m_stateRenderer = std::make_unique<Renderer::StateRenderer>();
    if(!m_stateRenderer->Initialize(this))
    {
        LOG_ERROR() << "Could not initialize state renderer!";
        return false;
    }

    // Initialize the editor system.
    // Built in editor for creating and modifying content within a game.
    m_editorSystem = std::make_unique<Editor::EditorSystem>();
    if(!m_editorSystem->Initialize(this))
    {
        LOG_ERROR() << "Could not initialize editor system!";
        return false;
    }

    // Success!
    return m_initialized = true;
}

bool Root::ProcessFrame()
{
    ASSERT(m_initialized, "Engine instance has not been initialized!");

    // Advance the logger frame counter of reference.
    Logger::AdvanceFrameCounter();

    // Signal engine exit.
    if(!m_window->IsOpen())
        return false;

    // Draw the editor system.
    m_editorSystem->Draw();

    // Present the window content.
    m_window->Present();

    // Release unused resources.
    m_resourceManager->ReleaseUnused();

    // Tick the timer.
    m_timer->Tick();

    // Calculate frame delta time.
    float timeDelta = m_timer->GetDeltaTime();

    // Prepare input state for being processed.
    m_inputState->PrepareForEvents();

    // Process window events.
    m_window->ProcessEvents();

    // Update the editor system.
    m_editorSystem->Update(timeDelta);

    // Signal that engine continues running.
    return true;
}

bool Root::IsInitialized() const
{
    return m_initialized;
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

System::InputState& Root::GetInputState()
{
    ASSERT(m_inputState);
    return *m_inputState;
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

Editor::EditorSystem& Root::GetEditorSystem()
{
    ASSERT(m_editorSystem);
    return *m_editorSystem;
}
