/*
    Copyright (c) 2018 Piotr Doan. All rights reserved.
*/

#include "Precompiled.hpp"
#include "Graphics/Buffer.hpp"
#include "Graphics/Context.hpp"
using namespace Graphics;

namespace
{
    // Constant definitions.
    const GLuint InvalidHandle = 0;
    const GLenum InvalidEnum = 0;
}

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

Buffer::Buffer(Context* context, GLenum type) :
    m_context(context),
    m_type(type),
    m_usage(GL_INVALID_ENUM),
    m_handle(InvalidHandle),
    m_elementSize(0),
    m_elementCount(0)
{
    VERIFY(m_context && m_context->IsValid(), "Graphics context is invalid!");
}

Buffer::~Buffer()
{
    this->DestroyHandle();
}

void Buffer::DestroyHandle()
{
    // Release the buffer's handle.
    if(m_handle != InvalidHandle)
    {
        glDeleteBuffers(1, &m_handle);
        m_handle = InvalidHandle;
    }
}

bool Buffer::Create(const BufferInfo& info)
{
    LOG() << "Creating " << this->GetName() << "..." << LOG_INDENT();

    // Check if the handle has been already created.
    VERIFY(m_handle == InvalidHandle, "Buffer instance has been already initialized!");

    // Validate that element size is not zero.
    // Element count can be zero for uninitialized buffers.
    if(info.elementSize == 0)
    {
        LOG_ERROR() << "Invalid argument - \"elementSize\" is 0!";
        return false;
    }

    // Setup a cleanup guard variable.
    bool initialized = false;

    // Create a buffer handle.
    SCOPE_GUARD_IF(!initialized, this->DestroyHandle());
    
    glGenBuffers(1, &m_handle);

    if(m_handle == InvalidHandle)
    {
        LOG_ERROR() << "Could not create a buffer handle!";
        return false;
    }

    // Allocate buffer memory.
    unsigned int bufferSize = info.elementSize * info.elementCount;

    if(bufferSize != 0)
    {
        glBindBuffer(m_type, m_handle);
        glBufferData(m_type, bufferSize, info.data, info.usage);
        glBindBuffer(m_type, m_context->GetState().GetBindBuffer(m_type));
    }

    // Save buffer parameters.
    m_elementSize = info.elementSize;
    m_elementCount = info.elementCount;
    m_usage = info.usage;

    LOG_INFO() << "Buffer size is " << bufferSize << " bytes.";

    // Success!
    LOG_INFO() << "Success!";

    return initialized = true;
}

void Buffer::Update(const void* data, int elementCount)
{
    VERIFY(m_handle != InvalidHandle, "Buffer handle has not been created!");
    VERIFY(data != nullptr, "Invalid argument - \"data\" is null!");
    VERIFY(elementCount > 0, "Invalid argument - \"elementCount\" is invalid!");

    // Upload new buffer data.
    glBindBuffer(m_type, m_handle);
    glBufferData(m_type, m_elementSize * elementCount, data, m_usage);
    glBindBuffer(m_type, m_context->GetState().GetBindBuffer(m_type));

    GLenum error = glGetError();
    ASSERT(error == GL_NO_ERROR, "Failed to upload buffer data!");
}

GLenum Buffer::GetType() const
{
    ASSERT(m_handle != InvalidHandle, "Buffer handle has not been created!");

    return m_type;
}

GLuint Buffer::GetHandle() const
{
    ASSERT(m_handle != InvalidHandle, "Buffer handle has not been created!");

    return m_handle;
}

unsigned int Buffer::GetElementSize() const
{
    ASSERT(m_handle != InvalidHandle, "Buffer handle has not been created!");

    return m_elementSize;
}

unsigned int Buffer::GetElementCount() const
{
    ASSERT(m_handle != InvalidHandle, "Buffer handle has not been created!");

    return m_elementCount;
}

GLenum Buffer::GetElementType() const
{
    ASSERT(m_handle != InvalidHandle, "Buffer handle has not been created!");

    return GL_INVALID_ENUM;
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

VertexBuffer::VertexBuffer(Context* context) :
    Buffer(context, GL_ARRAY_BUFFER)
{
}

const char* VertexBuffer::GetName() const
{
    return "vertex buffer";
}

/*
    Index Buffer
*/

IndexBuffer::IndexBuffer(Context* context) :
    Buffer(context, GL_ELEMENT_ARRAY_BUFFER)
{
}

const char* IndexBuffer::GetName() const
{
    return "index buffer";
}

GLenum IndexBuffer::GetElementType() const
{
    ASSERT(m_handle != InvalidHandle, "Buffer handle has not been created!");

    if(m_type == GL_ELEMENT_ARRAY_BUFFER)
    {
        switch(m_elementSize)
        {
            case 1: return GL_UNSIGNED_BYTE;
            case 2: return GL_UNSIGNED_SHORT;
            case 4: return GL_UNSIGNED_INT;
        }
    }

    return InvalidEnum;
}

/*
    Instance Buffer
*/

InstanceBuffer::InstanceBuffer(Context* context) :
    Buffer(context, GL_ARRAY_BUFFER)
{
}

const char* InstanceBuffer::GetName() const
{
    return "instance buffer";
}

bool InstanceBuffer::IsInstanced() const
{
    return true;
}
