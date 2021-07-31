/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#include "Graphics/Precompiled.hpp"
#include "Graphics/Buffer.hpp"
#include "Graphics/RenderContext.hpp"
using namespace Graphics;

/*
    Buffer
*/

Buffer::Buffer() = default;
Buffer::~Buffer()
{
    if(m_handle != OpenGL::InvalidHandle)
    {
        glDeleteBuffers(1, &m_handle);
        OpenGL::CheckErrors();
    }
}

Buffer::BufferResult Buffer::Initialize(GLenum type, const CreateFromParams& params)
{
    // Validate arguments.
    // Element count can be zero for uninitialized buffers.
    CHECK_ARGUMENT_OR_RETURN(type != OpenGL::InvalidEnum,
        Common::Failure(BufferErrors::InvalidArgument));
    CHECK_ARGUMENT_OR_RETURN(params.renderContext != nullptr,
        Common::Failure(BufferErrors::InvalidArgument));
    CHECK_ARGUMENT_OR_RETURN(params.elementSize != 0,
        Common::Failure(BufferErrors::InvalidArgument));

    // Create buffer handle.
    ASSERT(m_handle == OpenGL::InvalidHandle);

    glGenBuffers(1, &m_handle);
    OpenGL::CheckErrors();

    if(m_handle == OpenGL::InvalidHandle)
    {
        LOG_ERROR("Buffer handle could not be created!");
        return Common::Failure(BufferErrors::FailedResourceCreation);
    }

    // Allocate buffer memory.
    std::size_t bufferSize = params.elementSize * params.elementCount;

    if(bufferSize != 0)
    {
        glBindBuffer(type, m_handle);
        glBufferData(type, bufferSize, params.data, params.usage);
        glBindBuffer(type, params.renderContext->GetState().GetBufferBinding(type));
        OpenGL::CheckErrors();

        LOG_INFO("Uploaded {} bytes of buffer data.", bufferSize);
    }

    // Save buffer parameters.
    m_type = type;
    m_usage = params.usage;
    m_elementSize = params.elementSize;
    m_elementCount = params.elementCount;

    // Save render context reference.
    m_renderContext = params.renderContext;

    return Common::Success();
}

void Buffer::Update(const void* data, std::size_t elementCount)
{
    ASSERT_ALWAYS_ARGUMENT(data != nullptr);
    ASSERT_ALWAYS_ARGUMENT(elementCount > 0);

    // Upload new buffer data.
    glBindBuffer(m_type, m_handle);
    glBufferData(m_type, m_elementSize * elementCount, data, m_usage);
    glBindBuffer(m_type, m_renderContext->GetState().GetBufferBinding(m_type));
    OpenGL::CheckErrors();
}

/*
    Vertex Buffer
*/

VertexBuffer::VertexBuffer() = default;
VertexBuffer::~VertexBuffer() = default;

VertexBuffer::BufferResult VertexBuffer::Create(const Buffer::CreateFromParams& params)
{
    LOG_PROFILE_SCOPE("Create vertex buffer");

    // Create and initialize instance.
    auto instance = std::unique_ptr<VertexBuffer>(new VertexBuffer());
    if(auto initializeResult = instance->Initialize(GL_ARRAY_BUFFER, params).AsFailure())
    {
        LOG_ERROR("Failed to initialize vertex buffer!");
        return Common::Failure(initializeResult.Unwrap());
    }

    return Common::Success(std::move(instance));
}

/*
    Index Buffer
*/

IndexBuffer::IndexBuffer() = default;
IndexBuffer::~IndexBuffer() = default;

IndexBuffer::BufferResult IndexBuffer::Create(const Buffer::CreateFromParams& params)
{
    LOG_PROFILE_SCOPE("Create index buffer");

    // Create and initialize instance.
    auto instance = std::unique_ptr<IndexBuffer>(new IndexBuffer());
    if(auto initializeResult = instance->Initialize(GL_ELEMENT_ARRAY_BUFFER, params).AsFailure())
    {
        LOG_ERROR("Failed to initialize index buffer!");
        return Common::Failure(initializeResult.Unwrap());
    }

    return Common::Success(std::move(instance));
}

GLenum IndexBuffer::GetElementType() const
{
    if(m_type == GL_ELEMENT_ARRAY_BUFFER)
    {
        switch(m_elementSize)
        {
            case 1: return GL_UNSIGNED_BYTE;
            case 2: return GL_UNSIGNED_SHORT;
            case 4: return GL_UNSIGNED_INT;
        }
    }

    return GL_INVALID_ENUM;
}

/*
    Instance Buffer
*/

InstanceBuffer::InstanceBuffer() = default;
InstanceBuffer::~InstanceBuffer() = default;

InstanceBuffer::BufferResult InstanceBuffer::Create(const Buffer::CreateFromParams& params)
{
    LOG_PROFILE_SCOPE("Create instance buffer");

    // Create and initialize instance.
    auto instance = std::unique_ptr<InstanceBuffer>(new InstanceBuffer());
    if(auto initializeResult = instance->Initialize(GL_ARRAY_BUFFER, params).AsFailure())
    {
        LOG_ERROR("Failed to initialize buffer!");
        return Common::Failure(initializeResult.Unwrap());
    }

    return Common::Success(std::move(instance));
}
