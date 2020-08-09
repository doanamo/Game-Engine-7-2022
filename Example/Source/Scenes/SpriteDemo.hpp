/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#pragma once

#include <Game/GameState.hpp>
#include <Game/GameInstance.hpp>

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
        FailedGameInstanceCreation,
        FailedResourceLoading,
    };

    using CreateResult = Common::Result<std::unique_ptr<SpriteDemo>, CreateErrors>;
    static CreateResult Create(Engine::Root* engine);

public:
    ~SpriteDemo();

private:
    SpriteDemo();

    virtual void Update(float timeDelta) override;
    virtual void Tick(float timeDelta) override;
    virtual void Draw(float timeAlpha) override;

    virtual Game::GameInstance* GetGameInstance() const override;

private:
    Engine::Root* m_engine = nullptr;
    std::shared_ptr<Game::GameInstance> m_gameInstance;
};
