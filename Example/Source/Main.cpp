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
    while(engine.ProcessFrame())
    {
        const float updateTime = 1.0f / 10.0f;
        while(engine.timer.AdvanceFrame(updateTime))
        {
            gameScene.Update(updateTime);
        }

        Renderer::StateRenderer::DrawParams drawParams;
        drawParams.viewportRect = engine.window.GetViewportRect();
        drawParams.gameState = &gameScene.GetGameState();
        drawParams.cameraName = "Camera";
        drawParams.timeAlpha = 1.0f;

        engine.stateRenderer.Draw(drawParams);
    }

    return 0;
};
