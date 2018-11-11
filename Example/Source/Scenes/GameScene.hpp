/*
    Copyright (c) 2018 Piotr Doan. All rights reserved.
*/

#pragma once

#include <Scene/BaseScene.hpp>
#include <Graphics/ScreenSpace.hpp>

// Forward declarations.
namespace Engine
{
    class Root;
}

/*
    Game Scene

    Main scene used by the game.
*/

class GameScene : public Scene::BaseScene
{
public:
    GameScene();
    ~GameScene();

    // Disallow copying.
    GameScene(const GameScene& other) = delete;
    GameScene& operator=(const GameScene& other) = delete;

    // Move constructor and assignment.
    GameScene(GameScene&& other);
    GameScene& operator=(GameScene&& other);

    // Initializes the instance.
    bool Initialize(Engine::Root* engine) override;

    // Updates the scene.
    void OnUpdate(float timeDelta) override;

    // Draws the scene.
    void OnDraw(const Scene::SceneDrawParams& drawParams) override;

protected:
    // Enters the scene.
    void OnEnter() override;

    // Exits the scene.
    void OnExit() override;

private:
    // Initialization state.
    bool m_initialized;
};
