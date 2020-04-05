/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#pragma once

#include <Game/GameState.hpp>

// Forward declarations.
namespace Engine
{
    class Root;
}

namespace System
{
    class Timer;
}

/*
    Game Scene

    Main scene used by the game.
*/

class GameScene : public NonCopyable
{
public:
    GameScene();
    ~GameScene();

    // Move constructor and assignment.
    GameScene(GameScene&& other);
    GameScene& operator=(GameScene&& other);

    // Initializes the instance.
    bool Initialize(Engine::Root* engine);

private:
    // Updates the scene.
    void Update(float updateTime);

private:
    // Engine reference.
    Engine::Root* m_engine = nullptr;

    // Game state instance.
    std::shared_ptr<Game::GameState> m_gameState;

    // Custom update callback.
    Event::Receiver<void(float)> m_customUpdate;

    // Initialization state.
    bool m_initialized = false;
};
