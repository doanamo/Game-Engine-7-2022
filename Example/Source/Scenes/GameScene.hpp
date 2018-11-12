/*
    Copyright (c) 2018 Piotr Doan. All rights reserved.
*/

#pragma once

#include <Graphics/ScreenSpace.hpp>
#include <Game/GameState.hpp>

// Forward declarations.
namespace Engine
{
    class Root;
}

/*
    Game Scene

    Main scene used by the game.
*/

class GameScene
{
public:
    GameScene();
    ~GameScene();

    // Disallow copying.
    GameScene(const GameScene& other) = delete;
    GameScene& operator=(const GameScene& other) = delete;

    // Move constructor and assignment.
    GameScene(GameScene&& other);
    GameScene& operator=(GameScene&& other);

    // Initializes the instance.
    bool Initialize(Engine::Root* engine);

    // Updates the scene.
    void Update(float timeDelta);

    // Returns the game state.
    Game::GameState* GetGameState();

private:
    // Engine reference.
    Engine::Root* m_engine;

    // Game state.
    Game::GameState m_gameState;

    // Initialization state.
    bool m_initialized;
};
