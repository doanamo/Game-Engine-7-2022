/*
    Copyright (c) 2018 Piotr Doan. All rights reserved.
*/

#pragma once

#include "Graphics/RenderContext.hpp"
#include "Graphics/Buffer.hpp"
#include "Graphics/VertexArray.hpp"
#include "Graphics/Texture.hpp"
#include "Graphics/Sampler.hpp"
#include "Graphics/Shader.hpp"

// Forward declarations.
namespace Engine
{
    class Root;
}

/*
    Editor Renderer
*/

namespace Editor
{
    // Editor renderer class.
    class EditorRenderer
    {
    public:
        EditorRenderer();
        ~EditorRenderer();

        EditorRenderer(const EditorRenderer& other) = delete;
        EditorRenderer& operator=(const EditorRenderer& other) = delete;

        EditorRenderer(EditorRenderer&& other);
        EditorRenderer& operator=(EditorRenderer&& other);

        // Initializes the editor renderer.
        bool Initialize(Engine::Root* engine);

        // Draws the editor interface from the current context.
        void Draw();

    private:
        // Engine reference.
        Engine::Root* m_engine;

        // Graphics objects.
        Graphics::VertexBuffer m_vertexBuffer;
        Graphics::IndexBuffer m_indexBuffer;
        Graphics::VertexArray m_vertexArray;
        Graphics::Texture m_fontTexture;
        Graphics::Sampler m_sampler;
        Graphics::ShaderPtr m_shader;

        // Initialization state.
        bool m_initialized;
    };
}
