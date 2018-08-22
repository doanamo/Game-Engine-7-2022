/*
    Copyright (c) 2018 Piotr Doan. All rights reserved.
*/

#pragma once

#include "Game/Scene.hpp"
#include "Graphics/ScreenSpace.hpp"
#include "Graphics/Texture.hpp"

// Forward declarations.
namespace Engine
{
    class Root;
}

/*
    Texture Viewer
*/

namespace Editor
{
    // Texture viewer class.
    class TextureViewer : public Game::Scene
    {
    public:
        TextureViewer();
        ~TextureViewer();

        // Disallow copying.
        TextureViewer(const TextureViewer& other) = delete;
        TextureViewer& operator=(const TextureViewer& other) = delete;

        // Move constructor and assignment.
        TextureViewer(TextureViewer&& other);
        TextureViewer& operator=(TextureViewer&& other);

        // Initializes the editor scene.
        bool Initialize(Engine::Root* engine);

        // Updates the scene.
        void OnUpdate(float timeDelta) override;

        // Draws the scene.
        void OnDraw(float timeAlpha) override;

        // Returns the scene's name.
        const char* GetName() const override;

        // This scene implements a custom editor.
        bool HasCustomEditor() const override;

    private:
        // Engine reference.
        Engine::Root* m_engine;

        // Graphics objects.
        Graphics::ScreenSpace m_screenSpace;
        Graphics::TexturePtr m_texture;

        // Initialization state.
        bool m_initialized;
    };
}
