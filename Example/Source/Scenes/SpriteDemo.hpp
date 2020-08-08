/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#pragma once

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

class SpriteDemo final : private Common::NonCopyable
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

    Event::Receiver<void(float)> m_customTick;
    void Tick(float tickTime);

private:
    Engine::Root* m_engine = nullptr;

    std::shared_ptr<Game::GameInstance> m_gameInstance;
};
