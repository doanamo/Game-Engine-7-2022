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

class SpriteDemo final : public Game::GameState
{
public:
    enum class CreateErrors
    {
        InvalidArgument,
        FailedTickTimerCreation,
        FailedGameInstanceCreation,
        FailedResourceLoading,
    };

    using CreateResult = Common::Result<std::unique_ptr<SpriteDemo>, CreateErrors>;
    static CreateResult Create(Engine::Root* engine);

public:
    ~SpriteDemo();

private:
    SpriteDemo();

    virtual void Update(const float timeDelta) override;
    virtual void Tick(const float tickTime) override;
    virtual void Draw(const float timeAlpha) override;

    virtual Game::TickTimer* GetTickTimer() const override;
    virtual Game::GameInstance* GetGameInstance() const override;

private:
    Engine::Root* m_engine = nullptr;

    std::unique_ptr<Game::TickTimer> m_tickTimer;
    std::shared_ptr<Game::GameInstance> m_gameInstance;
};
