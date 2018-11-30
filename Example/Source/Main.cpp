/*
    Copyright (c) 2018 Piotr Doan. All rights reserved.
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

    // Main loop.
    engine.GetTimer().Reset();

    while(engine.ProcessFrame())
    {
        gameScene.Update(engine.GetTimer());

        Renderer::StateRenderer::DrawParams drawParams;
        drawParams.viewportRect = engine.GetWindow().GetViewportRect();
        drawParams.gameState = &gameScene.GetGameState();
        drawParams.cameraName = "Camera";

        engine.GetStateRenderer().Draw(drawParams);
    }

    return 0;
};
