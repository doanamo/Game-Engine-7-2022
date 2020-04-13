/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#pragma once

#include <Game/GameState.hpp>

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
*/

class GameScene : public NonCopyable
{
public:
    GameScene();
    ~GameScene();

    GameScene(GameScene&& other);
    GameScene& operator=(GameScene&& other);

    bool Initialize(Engine::Root* engine);

private:
    Event::Receiver<void(float)> m_customUpdate;
    void Update(float updateTime);

private:
    Engine::Root* m_engine = nullptr;

    std::shared_ptr<Game::GameState> m_gameState;

    bool m_initialized = false;
};
