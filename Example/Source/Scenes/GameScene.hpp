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

class GameScene final : private Common::NonCopyable
{
public:
    enum class CreateErrors
    {
        InvalidArgument,
        FailedGameStateCreation,
        FailedResourceLoading,
    };

    using CreateResult = Common::Result<std::unique_ptr<GameScene>, CreateErrors>;
    static CreateResult Create(Engine::Root* engine);

public:
    ~GameScene();

private:
    GameScene();

    Event::Receiver<void(float)> m_customUpdate;
    void Update(float updateTime);

private:
    Engine::Root* m_engine = nullptr;
    std::shared_ptr<Game::GameState> m_gameState;
};
