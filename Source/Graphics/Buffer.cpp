/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#include "Graphics/Buffer.hpp"
#include "Graphics/RenderContext.hpp"
using namespace Graphics;

/*
    Buffer
*/

Buffer::Buffer(GLenum type) :
    m_type(type)
{
}

Buffer::~Buffer()
{
    if(m_handle != OpenGL::InvalidHandle)
    {
        glDeleteBuffers(1, &m_handle);
        OpenGL::CheckErrors();
    }
}

Buffer::InitializeResult Buffer::Initialize(RenderContext* renderContext, const BufferInfo& info)
{
    LOG("Creating {}...", this->GetName());
    LOG_SCOPED_INDENT();

    // Check if instance has already been initialized.
    VERIFY(!m_initialized, "Instance has already been initialized!");

    // Validate arguments.
    // Element count can be zero for uninitialized buffers.
    CHECK_ARGUMENT_OR_RETURN(renderContext != nullptr, Failure(InitializeErrors::InvalidArgument));
    CHECK_ARGUMENT_OR_RETURN(info.elementSize != 0, Failure(InitializeErrors::InvalidArgument));

    // Create buffer handle.
    glGenBuffers(1, &m_handle);
    OpenGL::CheckErrors();

    if(m_handle == OpenGL::InvalidHandle)
    {
        LOG_ERROR("Buffer handle could not be created!");
        return Failure(InitializeErrors::FailedResourceCreation);
    }

    // Allocate buffer memory.
    std::size_t bufferSize = info.elementSize * info.elementCount;

    if(bufferSize != 0)
    {
        LOG_INFO("Uploading {} bytes of buffer data...", bufferSize);

        glBindBuffer(m_type, m_handle);
        glBufferData(m_type, bufferSize, info.data, info.usage);
        glBindBuffer(m_type, renderContext->GetState().GetBufferBinding(m_type));
        OpenGL::CheckErrors();
    }

    // Save buffer parameters.
    m_elementSize = info.elementSize;
    m_elementCount = info.elementCount;
    m_usage = info.usage;

    // Save render context reference.
    m_renderContext = renderContext;

    // Success!
    m_initialized = true;
    return Success();
}

void Buffer::Update(const void* data, std::size_t elementCount)
{
    ASSERT(m_initialized, "Buffer has not been initialized!");
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
    ASSERT(m_initialized, "Buffer has not been initialized!");
    return m_type;
}

GLuint Buffer::GetHandle() const
{
    ASSERT(m_initialized, "Buffer has not been initialized!");
    return m_handle;
}

std::size_t Buffer::GetElementSize() const
{
    ASSERT(m_initialized, "Buffer has not been initialized!");
    return m_elementSize;
}

std::size_t Buffer::GetElementCount() const
{
    ASSERT(m_initialized, "Buffer has not been initialized!");
    return m_elementCount;
}

bool Buffer::IsInitialized() const
{
    return m_initialized;
}

GLenum Buffer::GetElementType() const
{
    ASSERT(m_initialized, "Buffer has not been initialized!");
    return OpenGL::InvalidEnum;
}

bool Buffer::IsInstanced() const
{
    ASSERT(m_initialized, "Buffer has not been initialized!");
    return false;
}

/*
    Vertex Buffer
*/

VertexBuffer::VertexBuffer() :
    Buffer(GL_ARRAY_BUFFER)
{
}

VertexBuffer::~VertexBuffer() = default;

Buffer::InitializeResult VertexBuffer::Initialize(RenderContext* renderContext, const BufferInfo& info)
{
    // Setup initialization guard.
    VERIFY(!m_initialized, "Instance has already been initialized!");
    SCOPE_GUARD_IF(!m_initialized, this->Reset());

    // Initialize base class.
    SUCCESS_OR_RETURN_RESULT(Buffer::Initialize(renderContext, info));

    // Success!
    return Success();
}

const char* VertexBuffer::GetName() const
{
    return "vertex buffer";
}

/*
    Index Buffer
*/

IndexBuffer::IndexBuffer() :
    Buffer(GL_ELEMENT_ARRAY_BUFFER)
{
}

IndexBuffer::~IndexBuffer() = default;

Buffer::InitializeResult IndexBuffer::Initialize(RenderContext* renderContext, const BufferInfo& info)
{
    // Setup initialization guard.
    VERIFY(!m_initialized, "Instance has already been initialized!");
    SCOPE_GUARD_IF(!m_initialized, this->Reset());

    // Initialize base class.
    SUCCESS_OR_RETURN_RESULT(Buffer::Initialize(renderContext, info));

    // Success!
    return Success();
}

const char* IndexBuffer::GetName() const
{
    return "index buffer";
}

GLenum IndexBuffer::GetElementType() const
{
    ASSERT(m_handle != OpenGL::InvalidHandle, "Buffer handle has not been created!");

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

InstanceBuffer::InstanceBuffer() :
    Buffer(GL_ARRAY_BUFFER)
{
}

InstanceBuffer::~InstanceBuffer() = default;

Buffer::InitializeResult InstanceBuffer::Initialize(RenderContext* renderContext, const BufferInfo& info)
{
    // Setup initialization guard.
    VERIFY(!m_initialized, "Instance has already been initialized!");
    SCOPE_GUARD_IF(!m_initialized, this->Reset());

    // Initialize base class.
    SUCCESS_OR_RETURN_RESULT(Buffer::Initialize(renderContext, info));

    // Success!
    return Success();
}

const char* InstanceBuffer::GetName() const
{
    return "instance buffer";
}

bool InstanceBuffer::IsInstanced() const
{
    return true;
}
