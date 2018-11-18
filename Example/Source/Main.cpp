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
    GameScene gameScene;
    if(!gameScene.Initialize(&engine))
        return -1;

    // Main loop.
    engine.timer.Reset();

    while(engine.ProcessFrame())
    {
        gameScene.Update(engine.timer);

        Renderer::StateRenderer::DrawParams drawParams;
        drawParams.viewportRect = engine.window.GetViewportRect();
        drawParams.gameState = &gameScene.GetGameState();
        drawParams.cameraName = "Camera";

        engine.stateRenderer.Draw(drawParams);
    }

    return 0;
};
