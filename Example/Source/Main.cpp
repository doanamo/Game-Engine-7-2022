/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#include "Scenes/GameScene.hpp"

int main()
{
    // Create engine.
    Engine::Root engine;
    if(!engine.Initialize())
        return -1;

    // Create game scene.
    GameScene gameScene;
    if(!gameScene.Initialize(&engine))
        return -1;

    // Run example loop.
    return engine.Run();
};
