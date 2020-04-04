/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#pragma once

#include "Graphics/Buffer.hpp"
#include "Graphics/VertexArray.hpp"
#include "Graphics/Sampler.hpp"
#include "Graphics/Shader.hpp"
#include "Graphics/Sprite/Sprite.hpp"
#include "Graphics/Sprite/SpriteDrawList.hpp"

// Forward declarations.
namespace System
{
    class FileSystem;
    class ResourceManager;
}

/*
    Graphics Sprite Renderer
*/

namespace Graphics
{
    // Forward declarations.
    class RenderContext;

    // Sprite renderer class.
    class SpriteRenderer : private NonCopyable
    {
    public:
        // Initialization parameters.
        struct InitializeFromParams
        {
            System::FileSystem* fileSystem = nullptr;
            System::ResourceManager* resourceManager = nullptr;
            RenderContext* renderContext = nullptr;
            int spriteBatchSize = 128;
        };

    public:
        SpriteRenderer();
        ~SpriteRenderer();

        // Move constructor and assignment.
        SpriteRenderer(SpriteRenderer&& other);
        SpriteRenderer& operator=(SpriteRenderer&& other);

        // Initializes the sprite renderer.
        bool Initialize(const InitializeFromParams& params);

        // Draws a batch of sprites.
        // Very efficient rendering if array of sprites is already sorted to reduces state changes.
        void DrawSprites(const SpriteDrawList& sprites, const glm::mat4& transform);

    private:
        // Render context.
        RenderContext* m_renderContext;

        // Graphics objects.
        VertexBuffer m_vertexBuffer;
        InstanceBuffer m_instanceBuffer;
        VertexArray m_vertexArray;
        Sampler m_nearestSampler;
        Sampler m_linearSampler;
        std::shared_ptr<Shader> m_shader;

        // Sprite batch size.
        std::size_t m_spriteBatchSize;

        // Initialization state.
        bool m_initialized;
    };
}
