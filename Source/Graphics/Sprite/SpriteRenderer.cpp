/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#include "Graphics/Precompiled.hpp"
#include "Graphics/Sprite/SpriteRenderer.hpp"
#include "Graphics/RenderContext.hpp"
#include "Graphics/Texture.hpp"
#include <Core/SystemStorage.hpp>
#include <System/ResourceManager.hpp>
using namespace Graphics;

namespace
{
    struct SpriteVertex
    {
        glm::vec2 position;
        glm::vec2 texture;
    };
}

SpriteRenderer::SpriteRenderer() = default;
SpriteRenderer::~SpriteRenderer() = default;

bool Graphics::SpriteRenderer::OnAttach(const Core::EngineSystemStorage& engineSystems)
{
    // Locate required engine systems.
    auto* resourceManager = engineSystems.Locate<System::ResourceManager>();
    if(resourceManager == nullptr)
    {
        LOG_ERROR("Failed to locate resource manager system!");
        return false;
    }

    m_renderContext = engineSystems.Locate<Graphics::RenderContext>();
    if(m_renderContext == nullptr)
    {
        LOG_ERROR("Failed to locate render context system!");
        return false;
    }

    m_spriteBatchSize = 128;

    // Create vertex buffer.
    const SpriteVertex SpriteVertices[4] =
    {
        { glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, 0.0f) }, // Bottom-Left
        { glm::vec2(0.0f, 1.0f), glm::vec2(0.0f, 1.0f) }, // Bottom-Right
        { glm::vec2(1.0f, 0.0f), glm::vec2(1.0f, 0.0f) }, // Top-Left
        { glm::vec2(1.0f, 1.0f), glm::vec2(1.0f, 1.0f) }, // Top-Right
    };

    Buffer::CreateFromParams vertexBufferParams;
    vertexBufferParams.renderContext = m_renderContext;
    vertexBufferParams.usage = GL_STATIC_DRAW;
    vertexBufferParams.elementSize = sizeof(SpriteVertex);
    vertexBufferParams.elementCount = Common::StaticArraySize(SpriteVertices);
    vertexBufferParams.data = &SpriteVertices[0];

    m_vertexBuffer = VertexBuffer::Create(vertexBufferParams).UnwrapOr(nullptr);
    if(m_vertexBuffer == nullptr)
    {
        LOG_ERROR("Could not create vertex buffer!");
        return false;
    }

    // Create instance buffer.
    Buffer::CreateFromParams instanceBufferParams;
    instanceBufferParams.renderContext = m_renderContext;
    instanceBufferParams.usage = GL_STREAM_DRAW;
    instanceBufferParams.elementSize = sizeof(Sprite::Data);
    instanceBufferParams.elementCount = m_spriteBatchSize;
    instanceBufferParams.data = nullptr;

    m_instanceBuffer = InstanceBuffer::Create(instanceBufferParams).UnwrapOr(nullptr);
    if(m_instanceBuffer == nullptr)
    {
        LOG_ERROR("Could not create instance buffer!");
        return false;
    }

    // Create vertex array.
    const VertexArray::Attribute vertexAttributes[] =
    {
        { m_vertexBuffer.get(),   VertexArray::AttributeType::Vector2,   GL_FLOAT, false }, // Position
        { m_vertexBuffer.get(),   VertexArray::AttributeType::Vector2,   GL_FLOAT, false }, // Texture
        { m_instanceBuffer.get(), VertexArray::AttributeType::Matrix4x4, GL_FLOAT, false }, // Transform
        { m_instanceBuffer.get(), VertexArray::AttributeType::Vector4,   GL_FLOAT, false }, // Rectangle
        { m_instanceBuffer.get(), VertexArray::AttributeType::Vector4,   GL_FLOAT, false }, // Coordinates
        { m_instanceBuffer.get(), VertexArray::AttributeType::Vector4,   GL_FLOAT, false }, // Color
    };

    VertexArray::FromArrayParams vertexArrayParams;
    vertexArrayParams.attributeCount = Common::StaticArraySize(vertexAttributes);
    vertexArrayParams.attributes = &vertexAttributes[0];

    m_vertexArray = VertexArray::Create(m_renderContext, vertexArrayParams).UnwrapOr(nullptr);
    if(m_vertexArray == nullptr)
    {
        LOG_ERROR("Could not create vertex array!");
        return false;
    }

    // Create nearest sampler.
    Sampler::CreateFromParams nearestSamplerParams;
    nearestSamplerParams.renderContext = m_renderContext;
    nearestSamplerParams.textureMinFilter = GL_NEAREST;
    nearestSamplerParams.textureMagFilter = GL_NEAREST;

    m_nearestSampler = Sampler::Create(nearestSamplerParams).UnwrapOr(nullptr);
    if(m_nearestSampler == nullptr)
    {
        LOG_ERROR("Could not create nearest sampler!");
        return false;
    }

    // Create linear sampler.
    Sampler::CreateFromParams linearSamplerParams;
    linearSamplerParams.renderContext = m_renderContext;
    linearSamplerParams.textureMinFilter = GL_NEAREST_MIPMAP_LINEAR;
    linearSamplerParams.textureMagFilter = GL_LINEAR;

    m_linearSampler = Sampler::Create(linearSamplerParams).UnwrapOr(nullptr);
    if(m_linearSampler == nullptr)
    {
        LOG_ERROR("Could not create linear sampler!");
        return false;
    }

    // Load shader.
    Shader::LoadFromFile shaderParams;
    shaderParams.renderContext = m_renderContext;

    m_shader = resourceManager->Acquire<Shader>(
        "Data/Engine/Shaders/Sprite.shader", shaderParams)
        .UnwrapOr(nullptr);

    if(m_shader == nullptr)
    {
        LOG_ERROR("Could not load sprite shader!");
        return false;
    }

    return true;
}

void SpriteRenderer::DrawSprites(const SpriteDrawList& sprites, const glm::mat4& transform)
{
    // Push render state.
    auto& renderState = m_renderContext->PushState();
    SCOPE_GUARD([this]
    {
        m_renderContext->PopState();
    });

    // Set initial render state.
    renderState.BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    renderState.BindVertexArray(m_vertexArray->GetHandle());
    renderState.UseProgram(m_shader->GetHandle());

    // Set shader uniforms.
    m_shader->SetUniform("vertexTransform", transform);
    m_shader->SetUniform("textureDiffuse", 0);

    // Get sprite info and data arrays.
    const auto& spriteInfo = sprites.GetSpriteInfo();
    const auto& spriteData = sprites.GetSpriteData();

    // Render sprite batches.
    std::size_t spritesDrawn = 0;

    while(spritesDrawn < sprites.GetSpriteCount())
    {
        // Gets next sprite info to represent current batch.
        const Sprite::Info& batchInfo = spriteInfo[spritesDrawn];

        // Create batch of similar sprites.
        std::size_t spritesBatched = 1;

        while(spritesBatched <= m_spriteBatchSize)
        {
            // Get index of next sprite.
            std::size_t nextSprite = spritesDrawn + spritesBatched;

            // Check if we have already processed all sprites.
            if(nextSprite >= sprites.GetSpriteCount())
                break;

            // Check if sprites can be batched.
            if(batchInfo != spriteInfo[nextSprite])
                break;

            // Add sprite to batch.
            ++spritesBatched;
        }

        // Update buffer with sprite data and instances.
        m_instanceBuffer->Update(&spriteData[spritesDrawn], spritesBatched);

        // Set batch render state.
        if(batchInfo.transparent)
        {
            renderState.Enable(GL_BLEND);
            renderState.DepthMask(GL_FALSE);
        }
        else
        {
            renderState.Disable(GL_BLEND);
            renderState.DepthMask(GL_TRUE);
        }

        if(batchInfo.texture != nullptr)
        {
            // Bind texture unit.
            renderState.ActiveTexture(GL_TEXTURE0);
            renderState.BindTexture(GL_TEXTURE_2D, batchInfo.texture->GetHandle());

            // Bind texture sampler.
            if(batchInfo.filtered)
            {
                renderState.BindSampler(0, m_linearSampler->GetHandle());
            }
            else
            {
                renderState.BindSampler(0, m_nearestSampler->GetHandle());
            }
        }
        else
        {
            // Unbind texture unit.
            renderState.ActiveTexture(GL_TEXTURE0);
            renderState.BindTexture(GL_TEXTURE_2D, 0);
        }

        // Draw instanced sprite batch.
        glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, Common::NumericalCast<GLsizei>(spritesBatched));
        OpenGL::CheckErrors();

        // Update counter of drawn sprites.
        spritesDrawn += spritesBatched;
    }
}
