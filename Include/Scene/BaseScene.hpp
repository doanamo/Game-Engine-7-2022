/*
    Copyright (c) 2018 Piotr Doan. All rights reserved.
*/

#pragma once

#include "Scene/SceneInterface.hpp"
#include "Game/GameState.hpp"

// Forward declarations.
namespace Engine
{
    class Root;
}

/*
    Game Scene
*/

namespace Scene
{
    // Base scene base class.
    class BaseScene : public SceneInterface
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

        // Gets the game state.
        Game::GameState& GetGameState();

    private:
        // Engine reference.
        Engine::Root* m_engine;

        // Game state.
        Game::GameState m_gameState;

        // Initialization state.
        bool m_initialized;
    };
}
