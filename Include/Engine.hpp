/*
    Copyright (c) 2018 Piotr Doan. All rights reserved.
*/

#pragma once

#include "EnginePublic.hpp"
#include "System/Platform.hpp"
#include "System/Window.hpp"
#include "System/Timer.hpp"
#include "System/InputState.hpp"
#include "System/ResourceManager.hpp"
#include "Graphics/RenderContext.hpp"
#include "Graphics/SpriteRenderer.hpp"
#include "Game/EntitySystem.hpp"
#include "Game/ComponentSystem.hpp"
#include "Game/IdentitySystem.hpp"
#include "Editor/Editor.hpp"

/*
    Game Engine
*/

namespace Engine
{
    // Root class.
    class Root
    {
    public:
        Root();
        ~Root();

        // Disallow copying and moving.
        Root(const Root& other) = delete;
        Root& operator=(const Root& other) = delete;

        // Moving constructor and assignment.
        Root(Root&& other);
        Root& operator=(Root&& other);

        // Initializes the engine instance.
        bool Initialize();

        // Updates the engine before starting a new frame.
        // Must be called at the beginning of the main loop.
        bool Update();

        // Checks if the engine instance is initialized.
        bool IsInitialized() const;

    public:
        // Platform systems.
        System::Platform platform;
        System::Window window;
        System::Timer timer;
        System::InputState inputState;
        System::ResourceManager resourceManager;

        // Graphics systems.
        Graphics::RenderContext renderContext;
        Graphics::SpriteRenderer spriteRenderer;

        // Game systems.
        Game::EntitySystem entitySystem;
        Game::ComponentSystem componentSystem;
        Game::IdentitySystem identitySystem;

        // Engine systems.
        Engine::Editor editor;

    private:
        // Initialization state.
        bool m_initialized;

        // Track first update call.
        bool m_firstUpdate;
    };
}
