/*
    Copyright (c) 2018 Piotr Doan. All rights reserved.
*/

#include "Precompiled.hpp"
#include "Graphics/VertexArray.hpp"
#include "Graphics/Context.hpp"
#include "Graphics/Buffer.hpp"
using namespace Graphics;

namespace
{
    // Gets the row size of a vertex attribute type.
    int GetVertexAttributeTypeRowElements(VertexAttributeType type)
    {
        switch(type)
        {
        case VertexAttributeType::Value:
            return 1;

        case VertexAttributeType::Vector2:
            return 2;

        case VertexAttributeType::Vector3:
            return 3;

        case VertexAttributeType::Vector4:
            return 4;

        case VertexAttributeType::Matrix4x4:
            return 4;

        default:
            ASSERT(false, "Unknown vertex attribute type!");
            return 0;
        }
    }

    // Gets the row count of a vertex attribute type.
    int GetVertexAttributeTypeRowCount(VertexAttributeType type)
    {
        switch(type)
        {
        case VertexAttributeType::Value:
        case VertexAttributeType::Vector2:
        case VertexAttributeType::Vector3:
        case VertexAttributeType::Vector4:
            return 1;

        case VertexAttributeType::Matrix4x4:
            return 4;

        default:
            ASSERT(false, "Unknown vertex attribute type!");
            return 0;
        }
    }

    // Gets the size of an input data type.
    int GetVertexAttributeValueBytes(GLint value)
    {
        switch(value)
        {
        case GL_BYTE:
        case GL_UNSIGNED_BYTE:
            return 1;
            
        case GL_SHORT:
        case GL_UNSIGNED_SHORT:
            return 2;
            
        case GL_INT:
        case GL_UNSIGNED_INT:
            return 4;

        case GL_FLOAT:
            return 4;

        case GL_DOUBLE:
            return 8;

        default:
            ASSERT(false, "Unknown input data type!");
            return 0;
        }
    }
}

VertexAttribute::VertexAttribute() :
    buffer(nullptr),
    type(VertexAttributeType::Invalid),
    value(OpenGL::InvalidEnum),
    normalize(false)
{
}

VertexAttribute::VertexAttribute(const Buffer* buffer, VertexAttributeType type, GLenum value, bool normalize) :
    buffer(buffer),
    type(type),
    value(value),
    normalize(normalize)
{
}

VertexArrayInfo::VertexArrayInfo() :
    attributes(nullptr),
    attributeCount(0)
{
}

VertexArrayInfo::VertexArrayInfo(const VertexAttribute* attributes, int attributeCount) :
    attributes(attributes), attributeCount(attributeCount)
{
}

VertexArray::VertexArray(Context* context) :
    m_context(context),
    m_handle(OpenGL::InvalidHandle)
{
}

VertexArray::~VertexArray()
{
    this->DestroyHandle();
}

void VertexArray::DestroyHandle()
{
    // Release the vertex array handle.
    if(m_handle != OpenGL::InvalidHandle)
    {
        glDeleteVertexArrays(1, &m_handle);
        m_handle = OpenGL::InvalidHandle;
    }
}

bool VertexArray::Create(const VertexArrayInfo& info)
{
    LOG() << "Creating vertex input..." << LOG_INDENT();

    // Check if handle has been already created.
    VERIFY(m_handle == OpenGL::InvalidHandle, "Vertex array instance has been already initialized!");

    // Validate arguments.
    if(info.attributeCount <= 0)
    {
        LOG_ERROR() << "Invalid argument - \"count\" is zero!";
        return false;
    }

    if(info.attributes == nullptr)
    {
        LOG_ERROR() << "Invalid argument - \"attributes\" is null!";
        return false;
    }

    for(int i = 0; i < info.attributeCount; ++i)
    {
        const VertexAttribute& attribute = info.attributes[i];

        if(attribute.buffer == nullptr)
        {
            LOG_ERROR() << "Invalid argument - \"attribute[" << i << "].buffer\" is null!";
            return false;
        }

        if(!attribute.buffer->IsValid())
        {
            LOG_ERROR() << "Invalid argument - \"attribute[" << i << "].buffer\" is invalid!";
            return false;
        }

        if(attribute.buffer->GetType() != GL_ARRAY_BUFFER)
        {
            LOG_ERROR() << "Invalid argument - \"attribute[" << i << "].buffer\" is not a vertex or an instance buffer!";
            return false;
        }

        if(attribute.type == VertexAttributeType::Invalid)
        {
            LOG_ERROR() << "Invalid argument - \"attribute[" << i << "].storage\" is invalid!";
            return false;
        }

        if(attribute.value == OpenGL::InvalidEnum)
        {
            LOG_ERROR() << "Invalid argument - \"attribute[" << i << "].type\" is invalid!";
            return false;
        }
    }

    // Setup a cleanup guard variable.
    bool initialized = false;

    // Create a vertex array object.
    SCOPE_GUARD_IF(!initialized, this->DestroyHandle());

    glGenVertexArrays(1, &m_handle);

    if(m_handle == 0)
    {
        LOG_ERROR() << "Could not create a vertex array handle!";
        return false;
    }

    // Prepare a cleanup guard.
    SCOPE_GUARD_BEGIN();
    {
        glBindVertexArray(m_context->GetState().GetBindVertexArray());
        glBindBuffer(GL_ARRAY_BUFFER, m_context->GetState().GetBindBuffer(GL_ARRAY_BUFFER));
    }
    SCOPE_GUARD_END();

    // Bind the vertex array handle.
    glBindVertexArray(m_handle);

    // Setup the vertex attribute array.
    const Buffer* currentBuffer = nullptr;

    int currentLocation = 0;
    int currentOffset = 0;

    for(int i = 0; i < info.attributeCount; ++i)
    {
        const VertexAttribute& attribute = info.attributes[i];

        // Bind a vertex buffer.
        if(currentBuffer != attribute.buffer)
        {
            glBindBuffer(GL_ARRAY_BUFFER, attribute.buffer->GetHandle());

            currentBuffer = attribute.buffer;
            currentOffset = 0;
        }

        // Setup vertex attributes for each row of an input storage.
        for(int l = 0; l < GetVertexAttributeTypeRowCount(attribute.type); ++l)
        {
            // Enable vertex attribute.
            glEnableVertexAttribArray(currentLocation);

            // Set vertex attribute pointer.
            glVertexAttribPointer(
                currentLocation,
                GetVertexAttributeTypeRowElements(attribute.type),
                attribute.value,
                attribute.normalize ? GL_TRUE : GL_FALSE,
                attribute.buffer->GetElementSize(),
                (void*)currentOffset
            );

            // Make input location instanced.
            if(attribute.buffer->IsInstanced())
            {
                glVertexAttribDivisor(currentLocation, 1);
            }

            // Increment current location.
            currentLocation += 1;

            // Increment current offset.
            currentOffset += GetVertexAttributeValueBytes(attribute.value) * GetVertexAttributeTypeRowElements(attribute.type);
        }
    }

    // Success!
    LOG_INFO() << "Success!";

    return initialized = true;
}

GLuint VertexArray::GetHandle() const
{
    ASSERT(m_handle != OpenGL::InvalidHandle, "Vertex array handle has not been created!");

    return m_handle;
}

bool VertexArray::IsValid() const
{
    return m_handle != OpenGL::InvalidHandle;
}
