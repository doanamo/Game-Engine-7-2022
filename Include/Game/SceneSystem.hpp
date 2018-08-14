/*
    Copyright (c) 2018 Piotr Doan. All rights reserved.
*/

#pragma once

#include <memory>

// Forward declarations.
namespace Engine
{
    class Root;
}

/*
    Scene System
*/

namespace Game
{
    // Forward declarations.
    class Scene;

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
        void ChangeScene(std::shared_ptr<Scene> scene);

        // Updates the scene.
        void UpdateScene(float timeDelta);

        // Draws the scene.
        void DrawScene(float timeAlpha);

    private:
        // Engine reference.
        Engine::Root* m_engine;

        // Current scene.
        std::shared_ptr<Scene> m_scene;

        // Initialization state.
        bool m_initialized;
    };
}
