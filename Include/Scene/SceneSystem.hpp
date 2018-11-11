/*
    Copyright (c) 2018 Piotr Doan. All rights reserved.
*/

#pragma once

#include "Scene/SceneInterface.hpp"

// Forward declarations.
namespace Engine
{
    class Root;
}

/*
    Scene System
*/

namespace Scene
{
    // Forward declarations.
    class SceneInterface;

    // Scene system class.
    class SceneSystem
    {
    public:
        SceneSystem();
        ~SceneSystem();

        // Disallow copying.
        SceneSystem(const SceneSystem& other) = delete;
        SceneSystem& operator=(const SceneSystem& other) = delete;

        // Move constructor and assignment.
        SceneSystem(SceneSystem&& other);
        SceneSystem& operator=(SceneSystem&& other);

        // Initializes the scene system instance.
        bool Initialize(Engine::Root* engine);

        // Changes the current scene.
        void ChangeScene(std::shared_ptr<SceneInterface> scene);

        // Updates the current scene.
        void Update(float timeDelta);

        // Draws the current scene.
        void Draw(float timeAlpha);

    private:
        // Engine reference.
        Engine::Root* m_engine;

        // Current scene.
        std::shared_ptr<SceneInterface> m_currentScene;

        // Initialization state.
        bool m_initialized;
    };
}
