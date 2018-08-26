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
    Texture Editor
*/

namespace Editor
{
    // Texture editor class.
    class TextureEditor : public Game::Scene
    {
    public:
        TextureEditor();
        ~TextureEditor();

        // Disallow copying.
        TextureEditor(const TextureEditor& other) = delete;
        TextureEditor& operator=(const TextureEditor& other) = delete;

        // Move constructor and assignment.
        TextureEditor(TextureEditor&& other);
        TextureEditor& operator=(TextureEditor&& other);

        // Initializes the editor scene.
        bool Initialize(Engine::Root* engine);

        // Updates the scene.
        void OnUpdate(float timeDelta) override;

        // Draws the scene.
        void OnDraw(float timeAlpha) override;

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
