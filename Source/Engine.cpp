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
    
    std::swap(entitySystem, other.entitySystem);
    std::swap(componentSystem, other.componentSystem);
    std::swap(identitySystem, other.identitySystem);
    std::swap(sceneSystem, other.sceneSystem);

    std::swap(editor, other.editor);

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

    // Initialize the system platform context.
    // This will allow us to create and use platform systems such as window or input.
    if(!platform.Initialize())
    {
        LOG_ERROR() << "Could not initialize platform!";
        return false;
    }

    SCOPE_GUARD_IF(!m_initialized, platform = System::Platform());

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

    SCOPE_GUARD_IF(!m_initialized, window = System::Window());

    // Initialize the main timer.
    // There can be many timers but this one will be used to calculate frame time.
    if(!timer.Initialize())
    {
        LOG_ERROR() << "Could not initialize timer!";
        return false;
    }

    SCOPE_GUARD_IF(!m_initialized, timer = System::Timer());

    // Initialize the input state.
    // Collects and caches input state that can be later pooled.
    if(!inputState.Initialize(window))
    {
        LOG_ERROR() << "Could not initialize input state!";
        return false;
    }

    SCOPE_GUARD_IF(!m_initialized, inputState = System::InputState());

    // Initialize the resource manager.
    // Resource manager will help avoid duplication of resources.
    if(!resourceManager.Initialize())
    {
        LOG_ERROR() << "Could not initialize resource manager!";
        return false;
    }

    SCOPE_GUARD_IF(!m_initialized, resourceManager = System::ResourceManager());

    // Initialize the graphics context.
    // Manages the rendering context created along with the window.
    if(!renderContext.Initialize(&window))
    {
        LOG_ERROR() << "Could not initialize graphics context!";
        return false;
    }
    
    SCOPE_GUARD_IF(!m_initialized, renderContext = Graphics::RenderContext());

    // Initialize the sprite renderer.
    // Rendering subsystem for drawing sprites.
    if(!spriteRenderer.Initialize(&resourceManager, &renderContext, 128))
    {
        LOG_ERROR() << "Could not initialize sprite renderer!";
        return false;
    }

    SCOPE_GUARD_IF(!m_initialized, spriteRenderer = Graphics::SpriteRenderer());

    // Initialize the entity system.
    // Assigns unique identifiers that all other systems use to identify objects in a game.
    if(!entitySystem.Initialize())
    {
        LOG_ERROR() << "Could not initialize entity system!";
        return false;
    }

    SCOPE_GUARD_IF(!m_initialized, entitySystem = Game::EntitySystem());

    // Initialize the component system.
    // Stores and manages components that entities have.
    if(!componentSystem.Initialize(entitySystem))
    {
        LOG_ERROR() << "Could not initialize component system!";
        return false;
    }

    SCOPE_GUARD_IF(!m_initialized, componentSystem = Game::ComponentSystem());

    // Initialize the identity system.
    // Allows readable names to be assigned to entities.
    if(!identitySystem.Initialize(entitySystem))
    {
        LOG_ERROR() << "Could not initialize identity system!";
        return false;
    }

    SCOPE_GUARD_IF(!m_initialized, identitySystem = Game::IdentitySystem());

    // Initialize the scene system.
    // Allows game scenes to be switched and manages them.
    if(!sceneSystem.Initialize(this))
    {
        LOG_ERROR() << "Could not initialize scene system!";
        return false;
    }

    SCOPE_GUARD_IF(!m_initialized, sceneSystem = Game::SceneSystem());

    // Initialize the editor.
    // Built in editor for creating and modifying content within a game.
    if(!editor.Initialize(&window, &resourceManager, &renderContext))
    {
        LOG_ERROR() << "Could not initialize editor!";
        return false;
    }

    SCOPE_GUARD_IF(!m_initialized, editor = Engine::Editor());

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

        // Process entity commands.
        entitySystem.ProcessCommands();

        // Update the current scene.
        sceneSystem.UpdateScene(timeDelta);

        // Update the editor interface.
        editor.Update(timeDelta);

        // Draw the current scene.
        sceneSystem.DrawScene(1.0f);

        // Draw the editor interface.
        editor.Draw();

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
