/*
    Copyright (c) 2018 Piotr Doan. All rights reserved.
*/

#pragma once

#include "Game/Scene.hpp"
#include "Game/EntitySystem.hpp"
#include "Game/ComponentSystem.hpp"
#include "Game/IdentitySystem.hpp"

// Forward declarations.
namespace Engine
{
    class Root;
}

/*
    Game Scene
*/

namespace Game
{
    // Game scene base class.
    class GameScene : public Scene
    {
    protected:
        // Base constructor.
        GameScene();

        // Virtual destructor.
        virtual ~GameScene();

        // Disallow copying.
        GameScene(const GameScene& other) = delete;
        GameScene& operator=(const GameScene& other) = delete;

        // Move constructor and assignment.
        GameScene(GameScene&& other);
        GameScene& operator=(GameScene&& other);

        // Initializes the game scene.
        bool Initialize(Engine::Root* engine);

    public:
        // Updates the game scene.
        virtual void OnUpdate(float timeDelta) override;

        // Draws the game scene.
        virtual void OnDraw(float timeAlpha) override;

        // Gets the engine reference.
        Engine::Root* GetEngine() const;

    protected:
        // Base game systems.
        Game::EntitySystem m_entitySystem;
        Game::ComponentSystem m_componentSystem;
        Game::IdentitySystem m_identitySystem;

    private:
        // Engine reference.
        Engine::Root* m_engine;

        // Initialization state.
        bool m_initialized;
    };
}
