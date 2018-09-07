/*
    Copyright (c) 2018 Piotr Doan. All rights reserved.
*/

#pragma once

#include "Precompiled.hpp"
#include "Game/Scene.hpp"

// Forward declarations.
namespace Engine
{
    class Root;
}

/*
    Scene Renderer
*/

namespace Game
{
    // Forward declarations.
    class BaseScene;

    // Scene renderer class.
    class SceneRenderer
    {
    public:
        SceneRenderer();
        ~SceneRenderer();

        // Disallow copying.
        SceneRenderer(const SceneRenderer& other) = delete;
        SceneRenderer& operator=(const SceneRenderer& other) = delete;

        // Move constructor and assignment.
        SceneRenderer(SceneRenderer&& other);
        SceneRenderer& operator=(SceneRenderer&& other);

        // Initializes the scene renderer.
        bool Initialize(Engine::Root* engine);

        // Draws a scene.
        void DrawScene(Scene* scene, const SceneDrawParams& drawParams);

    private:
        // Engine reference.
        Engine::Root* m_engine;

        // Initialization state.
        bool m_initialized;
    };
}
