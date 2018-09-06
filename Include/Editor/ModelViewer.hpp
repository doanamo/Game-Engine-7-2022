/*
    Copyright (c) 2018 Piotr Doan. All rights reserved.
*/

#pragma once

#include "Game/BaseScene.hpp"
#include "Graphics/Texture.hpp"

// Forward declarations.
namespace Engine
{
    class Root;
}

/*
    Model Viewer
*/

namespace Editor
{
    // Model viewer class.
    class ModelViewer : public Game::BaseScene
    {
    public:
        ModelViewer();
        ~ModelViewer();

        // Disallow copying.
        ModelViewer(const ModelViewer& other) = delete;
        ModelViewer& operator=(const ModelViewer& other) = delete;

        // Move constructor and assignment.
        ModelViewer(ModelViewer&& other);
        ModelViewer& operator=(ModelViewer&& other);

        // Initializes the editor scene.
        bool Initialize(Engine::Root* engine) override;

        // Updates the scene.
        void OnUpdate(float timeDelta) override;

        // Draws the scene.
        void OnDraw(const Game::SceneDrawParams& drawParams) override;

    private:
        // Graphics objects.
        Graphics::TexturePtr m_texture;

        // Initialization state.
        bool m_initialized;
    };
}
