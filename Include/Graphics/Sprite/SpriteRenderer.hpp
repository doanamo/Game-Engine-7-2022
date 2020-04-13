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

namespace System
{
    class FileSystem;
    class ResourceManager;
}

/*
    Sprite Renderer
*/

namespace Graphics
{
    class RenderContext;

    class SpriteRenderer : private NonCopyable
    {
    public:
        struct InitializeFromParams
        {
            System::FileSystem* fileSystem = nullptr;
            System::ResourceManager* resourceManager = nullptr;
            RenderContext* renderContext = nullptr;
            int spriteBatchSize = 128;
        };

    public:
        SpriteRenderer() = default;
        ~SpriteRenderer() = default;

        SpriteRenderer(SpriteRenderer&& other);
        SpriteRenderer& operator=(SpriteRenderer&& other);

        bool Initialize(const InitializeFromParams& params);

        // Very efficient rendering if array of sprites is already sorted to reduces state changes.
        void DrawSprites(const SpriteDrawList& sprites, const glm::mat4& transform);

    private:
        RenderContext* m_renderContext = nullptr;
        std::size_t m_spriteBatchSize = 0;

        VertexBuffer m_vertexBuffer;
        InstanceBuffer m_instanceBuffer;
        VertexArray m_vertexArray;
        Sampler m_nearestSampler;
        Sampler m_linearSampler;
        std::shared_ptr<Shader> m_shader;

        bool m_initialized = false;
    };
}
