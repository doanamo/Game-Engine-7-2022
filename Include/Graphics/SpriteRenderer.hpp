/*
    Copyright (c) 2018 Piotr Doan. All rights reserved.
*/

#pragma once

#include <vector>
#include "Graphics/Buffer.hpp"
#include "Graphics/VertexArray.hpp"
#include "Graphics/Sampler.hpp"
#include "Graphics/Shader.hpp"
#include "Graphics/Sprite.hpp"
#include "Graphics/SpriteList.hpp"

/*
    Graphics Sprite Renderer
*/

namespace Graphics
{
    // Forward declarations.
    class RenderContext;

    // Sprite renderer class.
    class SpriteRenderer
    {
    public:
        SpriteRenderer();
        ~SpriteRenderer();

        // Disallow copying.
        SpriteRenderer(const SpriteRenderer& other) = delete;
        SpriteRenderer& operator=(const SpriteRenderer& other) = delete;

        // Move constructor and assignment.
        SpriteRenderer(SpriteRenderer&& other);
        SpriteRenderer& operator=(SpriteRenderer&& other);

        // Initializes the sprite renderer.
        bool Initialize(RenderContext* renderContext, int spriteBatchSize);

        // Draws a batch of sprites.
        // Very efficient rendering if array of sprites is already sorted to reduces state changes.
        void DrawSprites(const SpriteList& sprites, const glm::mat4& transform);

    private:
        // Render context.
        RenderContext* m_renderContext;

        // Graphics objects.
        VertexBuffer m_vertexBuffer;
        InstanceBuffer m_instanceBuffer;
        VertexArray m_vertexArray;
        Sampler m_nearestSampler;
        Sampler m_linearSampler;
        Shader m_shader;

        // Sprite batch size.
        std::size_t m_spriteBatchSize;

        // Initialization state.
        bool m_initialized;
    };
}
