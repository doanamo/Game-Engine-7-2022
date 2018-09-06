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
    // Base scene base class.
    class BaseScene : public Scene
    {
    protected:
        // Base constructor.
        BaseScene();

        // Virtual destructor.
        virtual ~BaseScene();

        // Disallow copying.
        BaseScene(const BaseScene& other) = delete;
        BaseScene& operator=(const BaseScene& other) = delete;

        // Move constructor and assignment.
        BaseScene(BaseScene&& other);
        BaseScene& operator=(BaseScene&& other);

        // Initializes the game scene.
        virtual bool Initialize(Engine::Root* engine);

    public:
        // Updates the game scene.
        virtual void OnUpdate(float timeDelta) override;

        // Draws the game scene.
        virtual void OnDraw(const SceneDrawParams& drawParams) override;

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
