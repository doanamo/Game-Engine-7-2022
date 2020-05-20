/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#pragma once

#include <Core/ServiceStorage.hpp>
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

    class SpriteRenderer final : private Common::NonCopyable
    {
    public:
        struct CreateFromParams
        {
            const Core::ServiceStorage* services = nullptr;
            int spriteBatchSize = 128;
        };

        enum class CreateErrors
        {
            InvalidArgument,
            FailedResourceCreation,
        };

        using CreateResult = Common::Result<std::unique_ptr<SpriteRenderer>, CreateErrors>;
        static CreateResult Create(const CreateFromParams& params);

    public:
        ~SpriteRenderer();

        void DrawSprites(const SpriteDrawList& sprites, const glm::mat4& transform);

    private:
        SpriteRenderer();

    private:
        RenderContext* m_renderContext = nullptr;
        std::size_t m_spriteBatchSize = 0;

        std::unique_ptr<VertexBuffer> m_vertexBuffer;
        std::unique_ptr<InstanceBuffer> m_instanceBuffer;
        std::unique_ptr<VertexArray> m_vertexArray;
        std::unique_ptr<Sampler> m_nearestSampler;
        std::unique_ptr<Sampler> m_linearSampler;
        std::shared_ptr<Shader> m_shader;
    };
}
