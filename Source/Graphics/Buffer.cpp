/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#include "Precompiled.hpp"
#include "Graphics/Buffer.hpp"
#include "Graphics/RenderContext.hpp"
using namespace Graphics;

/*
    Buffer
*/

BufferInfo::BufferInfo() :
    usage(GL_STATIC_DRAW),
    elementSize(0),
    elementCount(0),
    data(nullptr)
{
}

Buffer::Buffer(GLenum type) :
    m_renderContext(nullptr),
    m_type(type),
    m_usage(OpenGL::InvalidEnum),
    m_handle(OpenGL::InvalidHandle),
    m_elementSize(0),
    m_elementCount(0)
{
}

Buffer::Buffer(Buffer&& other) :
    Buffer(other.m_type)
{
    // Call the assignment operator.
    *this = std::move(other);
}

Buffer& Buffer::operator=(Buffer&& other)
{
    // Swap class members.
    std::swap(m_renderContext, other.m_renderContext);
    std::swap(m_type, other.m_type);
    std::swap(m_usage, other.m_usage);
    std::swap(m_handle, other.m_handle);
    std::swap(m_elementSize, other.m_elementSize);
    std::swap(m_elementCount, other.m_elementCount);

    return *this;
}

Buffer::~Buffer()
{
    this->DestroyHandle();
}

void Buffer::DestroyHandle()
{
    // Release the buffer's handle.
    if(m_handle != OpenGL::InvalidHandle)
    {
        glDeleteBuffers(1, &m_handle);
        OpenGL::CheckErrors();

        m_handle = OpenGL::InvalidHandle;
    }
}

bool Buffer::Initialize(RenderContext* renderContext, const BufferInfo& info)
{
    LOG("Creating {}...", this->GetName());
    LOG_SCOPED_INDENT();

    // Check if the handle has been already created.
    VERIFY(m_handle == OpenGL::InvalidHandle, "Buffer instance has been already initialized!");

    // Validate arguments.
    // Element count can be zero for uninitialized buffers.
    if(renderContext == nullptr)
    {
        LOG_ERROR("Invalid argument - \"renderContext\" is null!");
        return false;
    }

    if(info.elementSize == 0)
    {
        LOG_ERROR("Invalid argument - \"elementSize\" is 0!");
        return false;
    }

    // Setup a cleanup guard variable.
    bool initialized = false;

    // Create a buffer handle.
    SCOPE_GUARD_IF(!initialized, this->DestroyHandle());
    
    glGenBuffers(1, &m_handle);
    OpenGL::CheckErrors();

    if(m_handle == OpenGL::InvalidHandle)
    {
        LOG_ERROR("Buffer handle could not be created!");
        return false;
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
    return initialized = true;
}

void Graphics::Buffer::Update(const void* data, std::size_t elementCount)
{
    VERIFY(m_handle != OpenGL::InvalidHandle, "Buffer handle has not been created!");
    VERIFY(data != nullptr, "Invalid argument - \"data\" is null!");
    VERIFY(elementCount > 0, "Invalid argument - \"elementCount\" is invalid!");

    // Upload new buffer data.
    glBindBuffer(m_type, m_handle);
    glBufferData(m_type, m_elementSize * elementCount, data, m_usage);
    glBindBuffer(m_type, m_renderContext->GetState().GetBufferBinding(m_type));
    OpenGL::CheckErrors();
}

GLenum Buffer::GetType() const
{
    ASSERT(m_handle != OpenGL::InvalidHandle, "Buffer handle has not been created!");

    return m_type;
}

GLuint Buffer::GetHandle() const
{
    ASSERT(m_handle != OpenGL::InvalidHandle, "Buffer handle has not been created!");

    return m_handle;
}

std::size_t Graphics::Buffer::GetElementSize() const
{
    ASSERT(m_handle != OpenGL::InvalidHandle, "Buffer handle has not been created!");

    return m_elementSize;
}

std::size_t Graphics::Buffer::GetElementCount() const
{
    ASSERT(m_handle != OpenGL::InvalidHandle, "Buffer handle has not been created!");

    return m_elementCount;
}

GLenum Buffer::GetElementType() const
{
    ASSERT(m_handle != OpenGL::InvalidHandle, "Buffer handle has not been created!");

    return OpenGL::InvalidEnum;
}

bool Buffer::IsValid() const
{
    return m_handle != 0;
}

bool Buffer::IsInstanced() const
{
    return false;
}

/*
    Vertex Buffer
*/

VertexBuffer::VertexBuffer() :
    Buffer(GL_ARRAY_BUFFER)
{
}

VertexBuffer::VertexBuffer(VertexBuffer&& other) :
    Buffer(std::move(other))
{
}

VertexBuffer& VertexBuffer::operator=(VertexBuffer&& other)
{
    Buffer::operator=(std::move(other));
    return *this;
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

IndexBuffer::IndexBuffer(IndexBuffer&& other) :
    Buffer(std::move(other))
{
}

IndexBuffer& IndexBuffer::operator=(IndexBuffer&& other)
{
    Buffer::operator=(std::move(other));
    return *this;
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

InstanceBuffer::InstanceBuffer(InstanceBuffer&& other) :
    Buffer(std::move(other))
{
}

InstanceBuffer& InstanceBuffer::operator=(InstanceBuffer&& other)
{
    Buffer::operator=(std::move(other));
    return *this;
}


const char* InstanceBuffer::GetName() const
{
    return "instance buffer";
}

bool InstanceBuffer::IsInstanced() const
{
    return true;
}
