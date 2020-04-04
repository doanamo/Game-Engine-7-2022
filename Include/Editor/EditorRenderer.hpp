/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#pragma once

#include <Graphics/RenderContext.hpp>
#include <Graphics/Buffer.hpp>
#include <Graphics/VertexArray.hpp>
#include <Graphics/Texture.hpp>
#include <Graphics/Sampler.hpp>
#include <Graphics/Shader.hpp>

// Forward declarations.
namespace System
{
    class Window;
    class FileSystem;
    class ResourceManager;
}

namespace Graphics
{
    class RenderContext;
}

/*
    Editor Renderer
*/

namespace Editor
{
    // Editor renderer class.
    class EditorRenderer : private NonCopyable
    {
    public:
        struct InitializeFromParams
        {
            System::Window* window = nullptr;
            System::FileSystem* fileSystem = nullptr;
            System::ResourceManager* resourceManager = nullptr;
            Graphics::RenderContext* renderContext = nullptr;
        };

    public:
        EditorRenderer() = default;
        ~EditorRenderer() = default;

        EditorRenderer(EditorRenderer&& other);
        EditorRenderer& operator=(EditorRenderer&& other);

        // Initializes the editor renderer.
        bool Initialize(const InitializeFromParams& params);

        // Draws the editor interface from the current context.
        void Draw();

    private:
        // System references.
        System::Window* m_window = nullptr;
        Graphics::RenderContext* m_renderContext = nullptr;

        // Graphics objects.
        Graphics::VertexBuffer m_vertexBuffer;
        Graphics::IndexBuffer m_indexBuffer;
        Graphics::VertexArray m_vertexArray;
        Graphics::Texture m_fontTexture;
        Graphics::Sampler m_sampler;
        Graphics::ShaderPtr m_shader;

        // Initialization state.
        bool m_initialized = false;
    };
}
