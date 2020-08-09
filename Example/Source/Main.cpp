/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#include "Precompiled.hpp"
#include "Scenes/SpriteDemo.hpp"
#include <Game/GameFramework.hpp>

int main()
{
    // Create engine.
    Engine::Root::CreateFromParams engineParams;
    engineParams.maxTickDelta = 1.0f;

    auto engine = Engine::Root::Create(engineParams).UnwrapOr(nullptr);
    if(engine == nullptr)
    {
        LOG_ERROR("Could not create engine!");
        return -1;
    }

    // Create game state.
    std::shared_ptr<Game::GameState> gameState = SpriteDemo::Create(engine.get()).UnwrapOr(nullptr);
    if(gameState == nullptr)
    {
        LOG_ERROR("Could not create game state!");
        return -1;
    }

    // Set game state as current.
    engine->GetServices().GetGameFramework()->ChangeGameState(gameState);

    // Run example loop.
    return engine->Run();
};
