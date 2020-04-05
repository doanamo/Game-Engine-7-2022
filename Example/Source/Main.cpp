/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#include <Engine.hpp>
#include "Scenes/GameScene.hpp"

int main()
{
    // Create the engine.
    Engine::Root engine;
    if(!engine.Initialize())
        return -1;

    // Create the game scene.
    GameScene gameScene;
    if(!gameScene.Initialize(&engine))
        return -1;

    // Run the engine.
    return engine.Run();
};
