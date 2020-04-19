/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

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

Buffer::CreateResult Buffer::CreateBase(RenderContext* renderContext, GLenum type, const CreateFromParams& info)
{
    LOG("Creating buffer...");
    LOG_SCOPED_INDENT();

    // Validate arguments.
    // Element count can be zero for uninitialized buffers.
    CHECK_ARGUMENT_OR_RETURN(type != OpenGL::InvalidEnum, Failure(CreateErrors::InvalidArgument));
    CHECK_ARGUMENT_OR_RETURN(renderContext != nullptr, Failure(CreateErrors::InvalidArgument));
    CHECK_ARGUMENT_OR_RETURN(info.elementSize != 0, Failure(CreateErrors::InvalidArgument));

    // Create buffer handle.
    ASSERT(m_handle == OpenGL::InvalidHandle);

    glGenBuffers(1, &m_handle);
    OpenGL::CheckErrors();

    if(m_handle == OpenGL::InvalidHandle)
    {
        LOG_ERROR("Buffer handle could not be created!");
        return Failure(CreateErrors::FailedResourceCreation);
    }

    // Allocate buffer memory.
    std::size_t bufferSize = info.elementSize * info.elementCount;

    if(bufferSize != 0)
    {
        LOG_INFO("Uploading {} bytes of buffer data...", bufferSize);

        glBindBuffer(type, m_handle);
        glBufferData(type, bufferSize, info.data, info.usage);
        glBindBuffer(type, renderContext->GetState().GetBufferBinding(type));
        OpenGL::CheckErrors();
    }

    // Save buffer parameters.
    m_type = type;
    m_usage = info.usage;
    m_elementSize = info.elementSize;
    m_elementCount = info.elementCount;

    // Save render context reference.
    m_renderContext = renderContext;

    // Success!
    return Success();
}

void Buffer::Update(const void* data, std::size_t elementCount)
{
    VERIFY_ARGUMENT(data != nullptr);
    VERIFY_ARGUMENT(elementCount > 0);

    // Upload new buffer data.
    glBindBuffer(m_type, m_handle);
    glBufferData(m_type, m_elementSize * elementCount, data, m_usage);
    glBindBuffer(m_type, m_renderContext->GetState().GetBufferBinding(m_type));
    OpenGL::CheckErrors();
}

GLenum Buffer::GetType() const
{
    return m_type;
}

GLuint Buffer::GetHandle() const
{
    return m_handle;
}

std::size_t Buffer::GetElementSize() const
{
    return m_elementSize;
}

std::size_t Buffer::GetElementCount() const
{
    return m_elementCount;
}

GLenum Buffer::GetElementType() const
{
    return OpenGL::InvalidEnum;
}

bool Buffer::IsInstanced() const
{
    return false;
}

/*
    Vertex Buffer
*/

VertexBuffer::VertexBuffer() = default;
VertexBuffer::~VertexBuffer() = default;

VertexBuffer::CreateResult VertexBuffer::Create(RenderContext* renderContext, const Buffer::CreateFromParams& params)
{
    LOG("Creating vertex buffer...");
    LOG_SCOPED_INDENT();

    // Create instance.
    auto instance = std::unique_ptr<VertexBuffer>(new VertexBuffer());

    // Initialize base class.
    auto initializeResult = instance->CreateBase(renderContext, GL_ARRAY_BUFFER, params);

    if(!initializeResult)
    {
        return Failure(initializeResult.UnwrapFailure());
    }

    // Success!
    return Success(std::move(instance));
}

/*
    Index Buffer
*/

IndexBuffer::IndexBuffer() = default;
IndexBuffer::~IndexBuffer() = default;

IndexBuffer::CreateResult IndexBuffer::Create(RenderContext* renderContext, const Buffer::CreateFromParams& params)
{
    LOG("Creating index buffer...");
    LOG_SCOPED_INDENT();

    // Create instance.
    auto instance = std::unique_ptr<IndexBuffer>(new IndexBuffer());

    // Initialize base class.
    auto initializeResult = instance->CreateBase(renderContext, GL_ELEMENT_ARRAY_BUFFER, params);

    if(!initializeResult)
    {
        return Failure(initializeResult.UnwrapFailure());
    }

    // Success!
    return Success(std::move(instance));
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

InstanceBuffer::CreateResult InstanceBuffer::Create(RenderContext* renderContext, const Buffer::CreateFromParams& params)
{
    LOG("Creating index buffer...");
    LOG_SCOPED_INDENT();

    // Create instance.
    auto instance = std::unique_ptr<InstanceBuffer>(new InstanceBuffer());

    // Initialize base class.
    auto initializeResult = instance->CreateBase(renderContext, GL_ARRAY_BUFFER, params);

    if(!initializeResult)
    {
        return Failure(initializeResult.UnwrapFailure());
    }

    // Success!
    return Success(std::move(instance));
}

bool InstanceBuffer::IsInstanced() const
{
    return true;
}
