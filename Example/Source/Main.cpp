/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#include "Precompiled.hpp"
#include "Scenes/GameScene.hpp"
#include <System/Timer.hpp>
#include <System/Window.hpp>
#include <Renderer/StateRenderer.hpp>

int main()
{
    // Create the engine.
    Engine::InitializeParams initParams;
    initParams.maximumTickDelta = 1.0f;

    Engine::Root engine;
    if(!engine.Initialize(initParams))
        return -1;

    // Create the game scene.
    GameScene gameScene;
    if(!gameScene.Initialize(&engine))
        return -1;

    // Run the engine.
    return engine.Run();
};
