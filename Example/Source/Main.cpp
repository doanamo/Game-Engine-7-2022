/*
    Copyright (c) 2018 Piotr Doan. All rights reserved.
*/

#include "Precompiled.hpp"
#include "Scenes/GameScene.hpp"

int main()
{
    // Create the engine.
    Engine::Root engine;
    if(!engine.Initialize())
        return -1;

    // Create the game scene.
    auto CreateGameScene = [](Engine::Root* engine)
    {
        auto scene = std::make_shared<GameScene>();
        return scene->Initialize(engine) ? scene : nullptr;
    };

    engine.sceneSystem.ChangeScene(CreateGameScene(&engine));

    //
    /*
    while(engine.IsRunning())
    {
        while(engine.timer.AdvanceFrame(1.0f / 60.0f))
        {
            gameState.Update(1.0f / 60.0f);
        }

        engine.Draw(gameState);
    }
    */

    // Run the engine.
    return engine.Run();
};
