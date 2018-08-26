/*
    Copyright (c) 2018 Piotr Doan. All rights reserved.
*/

#include "Precompiled.hpp"
#include "Engine.hpp"
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
    // Call the move assignment.
    *this = std::move(other);
}

Root& Root::operator=(Root&& other)
{
    // Swap class members.
    std::swap(platform, other.platform);
    std::swap(window, other.window);
    std::swap(timer, other.timer);
    std::swap(inputState, other.inputState);
    std::swap(resourceManager, other.resourceManager);

    std::swap(renderContext, other.renderContext);
    std::swap(spriteRenderer, other.spriteRenderer);
    
    std::swap(sceneSystem, other.sceneSystem);
    std::swap(editorSystem, other.editorSystem);

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
    if(!platform.Initialize())
    {
        LOG_ERROR() << "Could not initialize platform!";
        return false;
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

    if(!window.Initialize(windowInfo))
    {
        LOG_ERROR() << "Could not initialize window!";
        return false;
    }

    // Initialize the main timer.
    // There can be many timers but this one will be used to calculate frame time.
    if(!timer.Initialize())
    {
        LOG_ERROR() << "Could not initialize timer!";
        return false;
    }

    // Initialize the input state.
    // Collects and caches input state that can be later pooled.
    if(!inputState.Initialize(window))
    {
        LOG_ERROR() << "Could not initialize input state!";
        return false;
    }

    // Initialize the resource manager.
    // Resource manager will help avoid duplication of resources.
    if(!resourceManager.Initialize())
    {
        LOG_ERROR() << "Could not initialize resource manager!";
        return false;
    }

    // Initialize the graphics context.
    // Manages the rendering context created along with the window.
    if(!renderContext.Initialize(&window))
    {
        LOG_ERROR() << "Could not initialize graphics context!";
        return false;
    }
    
    // Initialize the sprite renderer.
    // Rendering subsystem for drawing sprites.
    if(!spriteRenderer.Initialize(&resourceManager, &renderContext, 128))
    {
        LOG_ERROR() << "Could not initialize sprite renderer!";
        return false;
    }

    // Initialize the scene system.
    // Allows game scenes to be switched and manages them.
    if(!sceneSystem.Initialize(this))
    {
        LOG_ERROR() << "Could not initialize scene system!";
        return false;
    }

    SCOPE_GUARD_IF(!m_initialized, sceneSystem = Game::SceneSystem());

    // Initialize the editor system.
    // Built in editor for creating and modifying content within a game.
    if(!editorSystem.Initialize(this))
    {
        LOG_ERROR() << "Could not initialize editor system!";
        return false;
    }

    SCOPE_GUARD_IF(!m_initialized, editorSystem = Editor::EditorSystem());

    // Success!
    return m_initialized = true;
}

int Root::Run()
{
    ASSERT(m_initialized, "Engine instance has not been initialized!");

    // Reset the timer before the first update, as a large
    // value may have accumulated after a long initialization.
    timer.Reset();

    // Run the main application loop.
    while(window.IsOpen())
    {
        // Calculate frame delta time.
        float timeDelta = timer.CalculateFrameDelta();

        // Prepare input state for being processed.
        inputState.PrepareForEvents();

        // Process window events.
        window.ProcessEvents();

        // Update the editor system.
        editorSystem.Update(timeDelta);

        // Update the current scene.
        sceneSystem.Update(timeDelta);

        // Clear the frame buffer.
        renderContext.GetState().Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Draw the current scene.
        sceneSystem.Draw(1.0f);

        // Draw the editor system.
        editorSystem.Draw();

        // Present the window content.
        window.Present();

        // Release unused resources.
        resourceManager.ReleaseUnused();

        // Tick the timer.
        timer.Tick();
    }

    return 0;
}

bool Root::IsInitialized() const
{
    return m_initialized;
}
