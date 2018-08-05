/*
    Copyright (c) 2018 Piotr Doan. All rights reserved.
*/

#include "Precompiled.hpp"
#include "Graphics/SpriteRenderer.hpp"
#include "Graphics/RenderContext.hpp"
#include "Graphics/Texture.hpp"
using namespace Graphics;

namespace
{
    // Sprite vertex structure.
    struct SpriteVertex
    {
        glm::vec2 position;
        glm::vec2 texture;
    };
}

SpriteRenderer::SpriteRenderer() :
    m_renderContext(nullptr),
    m_spriteBatchSize(0),
    m_initialized(false)
{
}

SpriteRenderer::~SpriteRenderer()
{
}

SpriteRenderer::SpriteRenderer(SpriteRenderer&& other) :
    SpriteRenderer()
{
    // Call the move assignment.
    *this = std::move(other);
}

SpriteRenderer& SpriteRenderer::operator=(SpriteRenderer&& other)
{
    // Swap class members.
    std::swap(m_renderContext, other.m_renderContext);
    std::swap(m_vertexBuffer, other.m_vertexBuffer);
    std::swap(m_instanceBuffer, other.m_instanceBuffer);
    std::swap(m_vertexArray, other.m_vertexArray);
    std::swap(m_nearestSampler, other.m_nearestSampler);
    std::swap(m_linearSampler, other.m_linearSampler);
    std::swap(m_shader, other.m_shader);
    std::swap(m_spriteBatchSize, other.m_spriteBatchSize);
    std::swap(m_initialized, other.m_initialized);

    return *this;
}

bool SpriteRenderer::Initialize(RenderContext* renderContext, int spriteBatchSize)
{
    LOG() << "Initializing sprite renderer..." << LOG_INDENT();

    // Make sure that the instance is not already initialized.
    ASSERT(!m_initialized, "Sprite renderer instance has already been initialized!");

    // Validate arguments.
    if(renderContext == nullptr)
    {
        LOG_ERROR() << "Invalid argument - \"renderContext\" is null!";
        return false;
    }

    if(spriteBatchSize <= 0)
    {
        LOG_ERROR() << "Invalid argument - \"spriteBatchSize\" is zero or less!";
        return false;
    }

    // Create the vertex buffer.
    const SpriteVertex SpriteVertices[4] =
    {
        { glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, 0.0f) }, // Bottom-Left
        { glm::vec2(0.0f, 1.0f), glm::vec2(0.0f, 1.0f) }, // Bottom-Right
        { glm::vec2(1.0f, 0.0f), glm::vec2(1.0f, 0.0f) }, // Top-Left
        { glm::vec2(1.0f, 1.0f), glm::vec2(1.0f, 1.0f) }, // Top-Right
    };

    BufferInfo vertexBufferInfo;
    vertexBufferInfo.usage = GL_STATIC_DRAW;
    vertexBufferInfo.elementSize = sizeof(SpriteVertex);
    vertexBufferInfo.elementCount = Utility::StaticArraySize(SpriteVertices);
    vertexBufferInfo.data = &SpriteVertices[0];

    if(!m_vertexBuffer.Initialize(renderContext, vertexBufferInfo))
    {
        LOG_ERROR() << "Could not create vertex buffer!";
        return false;
    }

    SCOPE_GUARD_IF(!m_initialized, m_vertexBuffer = VertexBuffer());

    // Create the instance buffer.
    BufferInfo instanceBufferInfo;
    instanceBufferInfo.usage = GL_STREAM_DRAW;
    instanceBufferInfo.elementSize = sizeof(Sprite::Data);
    instanceBufferInfo.elementCount = spriteBatchSize;
    instanceBufferInfo.data = nullptr;

    if(!m_instanceBuffer.Initialize(renderContext, instanceBufferInfo))
    {
        LOG_ERROR() << "Could not create instance buffer!";
        return false;
    }

    SCOPE_GUARD_IF(!m_initialized, m_instanceBuffer = InstanceBuffer());

    // Create the vertex array.
    const VertexAttribute vertexAttributes[] =
    {
        { &m_vertexBuffer,   VertexAttributeType::Vector2,   GL_FLOAT, false }, // Position
        { &m_vertexBuffer,   VertexAttributeType::Vector2,   GL_FLOAT, false }, // Texture
        { &m_instanceBuffer, VertexAttributeType::Matrix4x4, GL_FLOAT, false }, // Transform
        { &m_instanceBuffer, VertexAttributeType::Vector4,   GL_FLOAT, false }, // Sprite Rectangle
        { &m_instanceBuffer, VertexAttributeType::Vector4,   GL_FLOAT, false }, // Texture Rectangle
        { &m_instanceBuffer, VertexAttributeType::Vector4,   GL_FLOAT, false }, // Color
    };

    Graphics::VertexArrayInfo vertexArrayInfo;
    vertexArrayInfo.attributeCount = Utility::StaticArraySize(vertexAttributes);
    vertexArrayInfo.attributes = &vertexAttributes[0];

    if(!m_vertexArray.Initialize(renderContext, vertexArrayInfo))
    {
        LOG_ERROR() << "Could not create vertex array!";
        return false;
    }

    SCOPE_GUARD_IF(!m_initialized, m_vertexArray = VertexArray());

    // Create a nearest sampler.
    SamplerInfo nearestSamplerInfo;
    nearestSamplerInfo.textureMinFilter = GL_NEAREST;
    nearestSamplerInfo.textureMagFilter = GL_NEAREST;

    if(!m_nearestSampler.Initialize(renderContext, nearestSamplerInfo))
    {
        LOG_ERROR() << "Could not create a nearest sampler!";
        return false;
    }

    SCOPE_GUARD_IF(!m_initialized, m_nearestSampler = Sampler());

    // Create a linear sampler.
    SamplerInfo linearSamplerInfo;
    linearSamplerInfo.textureMinFilter = GL_NEAREST_MIPMAP_LINEAR;
    linearSamplerInfo.textureMagFilter = GL_LINEAR;

    if(!m_linearSampler.Initialize(renderContext, linearSamplerInfo))
    {
        LOG_ERROR() << "Could not create linear sampler!";
        return false;
    }

    SCOPE_GUARD_IF(!m_initialized, m_linearSampler = Sampler());

    // Load the shader.
    ShaderLoadInfo shaderInfo;
    shaderInfo.filePath = Build::GetEngineDir() + "Data/Engine/Shaders/Sprite.shader";

    if(!m_shader.Initialize(renderContext, shaderInfo))
    {
        LOG_ERROR() << "Could not load sprite shader!";
        return false;
    }

    SCOPE_GUARD_IF(!m_initialized, m_shader = Shader());

    // Remember the sprite batch size.
    m_spriteBatchSize = spriteBatchSize;

    // Save render context reference.
    m_renderContext = renderContext;

    // Success!
    return m_initialized = true;
}

void SpriteRenderer::DrawSprites(const SpriteList& sprites, const glm::mat4& transform)
{
    ASSERT(m_initialized, "Sprite renderer has not been initialized!");

    // Get the render state.
    RenderState& renderState = m_renderContext->GetState();

    // Push render state.
    m_renderContext->PushState();
    SCOPE_GUARD(m_renderContext->PopState());

    // Set initial render state.
    renderState.BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    renderState.BindVertexArray(m_vertexArray.GetHandle());
    renderState.UseProgram(m_shader.GetHandle());

    // Set shader uniforms.
    m_shader.SetUniform("vertexTransform", transform);
    m_shader.SetUniform("textureDiffuse", 0);

    // Get sprite info and data arrays.
    const auto& spriteInfo = sprites.GetSpriteInfo();
    const auto& spriteData = sprites.GetSpriteData();

    // Render sprite batches.
    std::size_t spritesDrawn = 0;

    while(spritesDrawn < sprites.GetSpriteCount())
    {
        // Gets the next sprite info to represent the current batch.
        const Sprite::Info& batchInfo = spriteInfo[spritesDrawn];

        // Create a batch of similar sprites.
        std::size_t spritesBatched = 1;

        while(spritesBatched <= m_spriteBatchSize)
        {
            // Get the index of the next sprite.
            std::size_t nextSprite = spritesDrawn + spritesBatched;

            // Check if we have already processed all sprites.
            if(nextSprite >= sprites.GetSpriteCount())
                break;

            // Check if sprites can be batched.
            if(batchInfo != spriteInfo[nextSprite])
                break;

            // Add sprite to the batch.
            ++spritesBatched;
        }

        // Update instance buffer with sprite data.
        m_instanceBuffer.Update(&spriteData[spritesDrawn], spritesBatched);

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
            // Calculate and set inversed texture size.
            glm::vec2 textureInvSize;
            textureInvSize.x = 1.0f / batchInfo.texture->GetWidth();
            textureInvSize.y = 1.0f / batchInfo.texture->GetHeight();

            m_shader.SetUniform("textureSizeInv", textureInvSize);

            // Bind texture unit.
            renderState.ActiveTexture(GL_TEXTURE0);
            renderState.BindTexture(GL_TEXTURE_2D, batchInfo.texture->GetHandle());

            // Bind texture sampler.
            if(batchInfo.filter)
            {
                renderState.BindSampler(0, m_linearSampler.GetHandle());
            }
            else
            {
                renderState.BindSampler(0, m_nearestSampler.GetHandle());
            }
        }
        else
        {
            // Unbind texture unit.
            renderState.ActiveTexture(GL_TEXTURE0);
            renderState.BindTexture(GL_TEXTURE_2D, 0);
        }

        // Draw instanced sprite batch.
        glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, spritesBatched);
        OpenGL::CheckErrors();

        // Update the counter of drawn sprites.
        spritesDrawn += spritesBatched;
    }
}
