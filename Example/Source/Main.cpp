/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#include "Precompiled.hpp"
#include "Scenes/GameScene.hpp"

int main()
{
    // Create engine.
    Engine::Root::CreateFromParams engineParams;
    engineParams.maximumTickDelta = 1.0f;

    auto engine = Engine::Root::Create(engineParams).UnwrapOr(nullptr);
    if(engine == nullptr)
    {
        LOG_ERROR("Could not create engine!");
        return -1;
    }

    // Create game scene.
    auto gameScene = GameScene::Create(engine.get()).UnwrapOr(nullptr);
    if(gameScene == nullptr)
    {
        LOG_ERROR("Could not create game scene!");
        return -1;
    }

    // Run example loop.
    return engine->Run();
};
