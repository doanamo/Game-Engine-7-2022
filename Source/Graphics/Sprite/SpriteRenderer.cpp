/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#include "Graphics/Sprite/SpriteRenderer.hpp"
#include "Graphics/RenderContext.hpp"
#include "Graphics/Texture.hpp"
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

SpriteRenderer::CreateResult SpriteRenderer::Create(const CreateFromParams& params)
{
    LOG("Creating sprite renderer...");
    LOG_SCOPED_INDENT();

    // Check arguments.
    CHECK_ARGUMENT_OR_RETURN(params.fileSystem != nullptr, Failure(CreateErrors::InvalidArgument));
    CHECK_ARGUMENT_OR_RETURN(params.resourceManager != nullptr, Failure(CreateErrors::InvalidArgument));
    CHECK_ARGUMENT_OR_RETURN(params.renderContext != nullptr, Failure(CreateErrors::InvalidArgument));
    CHECK_ARGUMENT_OR_RETURN(params.spriteBatchSize > 0, Failure(CreateErrors::InvalidArgument));

    // Create instance.
    auto instance = std::unique_ptr<SpriteRenderer>(new SpriteRenderer());

    // Create vertex buffer.
    const SpriteVertex SpriteVertices[4] =
    {
        { glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, 0.0f) }, // Bottom-Left
        { glm::vec2(0.0f, 1.0f), glm::vec2(0.0f, 1.0f) }, // Bottom-Right
        { glm::vec2(1.0f, 0.0f), glm::vec2(1.0f, 0.0f) }, // Top-Left
        { glm::vec2(1.0f, 1.0f), glm::vec2(1.0f, 1.0f) }, // Top-Right
    };

    Buffer::CreateFromParams vertexBufferParams;
    vertexBufferParams.usage = GL_STATIC_DRAW;
    vertexBufferParams.elementSize = sizeof(SpriteVertex);
    vertexBufferParams.elementCount = Utility::StaticArraySize(SpriteVertices);
    vertexBufferParams.data = &SpriteVertices[0];

    instance->m_vertexBuffer = VertexBuffer::Create(params.renderContext, vertexBufferParams).UnwrapOr(nullptr);
    if(instance->m_vertexBuffer == nullptr)
    {
        LOG_ERROR("Could not create vertex buffer!");
        return Failure(CreateErrors::FailedResourceCreation);
    }

    // Create instance buffer.
    Buffer::CreateFromParams instanceBufferParams;
    instanceBufferParams.usage = GL_STREAM_DRAW;
    instanceBufferParams.elementSize = sizeof(Sprite::Data);
    instanceBufferParams.elementCount = params.spriteBatchSize;
    instanceBufferParams.data = nullptr;

    instance->m_instanceBuffer = InstanceBuffer::Create(params.renderContext, instanceBufferParams).UnwrapOr(nullptr);
    if(instance->m_instanceBuffer == nullptr)
    {
        LOG_ERROR("Could not create instance buffer!");
        return Failure(CreateErrors::FailedResourceCreation);
    }

    // Create vertex array.
    const VertexArray::Attribute vertexAttributes[] =
    {
        { instance->m_vertexBuffer.get(),   VertexArray::AttributeType::Vector2,   GL_FLOAT, false }, // Position
        { instance->m_vertexBuffer.get(),   VertexArray::AttributeType::Vector2,   GL_FLOAT, false }, // Texture
        { instance->m_instanceBuffer.get(), VertexArray::AttributeType::Matrix4x4, GL_FLOAT, false }, // Transform
        { instance->m_instanceBuffer.get(), VertexArray::AttributeType::Vector4,   GL_FLOAT, false }, // Rectangle
        { instance->m_instanceBuffer.get(), VertexArray::AttributeType::Vector4,   GL_FLOAT, false }, // Coordinates
        { instance->m_instanceBuffer.get(), VertexArray::AttributeType::Vector4,   GL_FLOAT, false }, // Color
    };

    Graphics::VertexArray::FromArrayParams vertexArrayParams;
    vertexArrayParams.attributeCount = Utility::StaticArraySize(vertexAttributes);
    vertexArrayParams.attributes = &vertexAttributes[0];

    instance->m_vertexArray = Graphics::VertexArray::Create(params.renderContext, vertexArrayParams).UnwrapOr(nullptr);
    if(instance->m_vertexArray == nullptr)
    {
        LOG_ERROR("Could not create vertex array!");
        return Failure(CreateErrors::FailedResourceCreation);
    }

    // Create nearest sampler.
    Sampler::CreateFromParams nearestSamplerParams;
    nearestSamplerParams.textureMinFilter = GL_NEAREST;
    nearestSamplerParams.textureMagFilter = GL_NEAREST;

    instance->m_nearestSampler = Sampler::Create(params.renderContext, nearestSamplerParams).UnwrapOr(nullptr);
    if(instance->m_nearestSampler == nullptr)
    {
        LOG_ERROR("Could not create nearest sampler!");
        return Failure(CreateErrors::FailedResourceCreation);
    }

    // Create linear sampler.
    Sampler::CreateFromParams linearSamplerParams;
    linearSamplerParams.textureMinFilter = GL_NEAREST_MIPMAP_LINEAR;
    linearSamplerParams.textureMagFilter = GL_LINEAR;

    instance->m_linearSampler = Sampler::Create(params.renderContext, linearSamplerParams).UnwrapOr(nullptr);
    if(instance->m_linearSampler == nullptr)
    {
        LOG_ERROR("Could not create linear sampler!");
        return Failure(CreateErrors::FailedResourceCreation);
    }

    // Load shader.
    Shader::LoadFromFile shaderParams;
    shaderParams.fileSystem = params.fileSystem;
    shaderParams.filePath = "Data/Engine/Shaders/Sprite.shader";
    shaderParams.renderContext = params.renderContext;

    instance->m_shader = params.resourceManager->Acquire<Shader>(
        shaderParams.filePath, shaderParams).UnwrapOr(nullptr);

    if(instance->m_shader == nullptr)
    {
        LOG_ERROR("Could not load sprite shader!");
        return Failure(CreateErrors::FailedResourceCreation);
    }

    // Remember sprite batch size.
    instance->m_spriteBatchSize = params.spriteBatchSize;

    // Save render context reference.
    instance->m_renderContext = params.renderContext;

    // Success!
    return Success(std::move(instance));
}

void SpriteRenderer::DrawSprites(const SpriteDrawList& sprites, const glm::mat4& transform)
{
    // Push render state.
    auto& renderState = m_renderContext->PushState();
    SCOPE_GUARD(m_renderContext->PopState());

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
        glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, Utility::NumericalCast<GLsizei>(spritesBatched));
        OpenGL::CheckErrors();

        // Update counter of drawn sprites.
        spritesDrawn += spritesBatched;
    }
}
