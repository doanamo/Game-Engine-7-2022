/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#pragma once

#include <Core/EngineSystem.hpp>
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

    class SpriteRenderer final : public Core::EngineSystem
    {
        REFLECTION_ENABLE(SpriteRenderer, Core::EngineSystem)

    public:
        SpriteRenderer();
        ~SpriteRenderer() override;

        void DrawSprites(const SpriteDrawList& sprites, const glm::mat4& transform);

    private:
        bool OnAttach(const Core::EngineSystemStorage& engineSystems) override;

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

REFLECTION_TYPE(Graphics::SpriteRenderer, Core::EngineSystem)
