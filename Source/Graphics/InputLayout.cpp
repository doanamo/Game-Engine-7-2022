/*
    Copyright (c) 2018 Piotr Doan. All rights reserved.
*/

#include "Precompiled.hpp"
#include "Graphics/InputLayout.hpp"
#include "Graphics/Buffer.hpp"
using namespace Graphics;

namespace
{
    // Gets the row size of an input storage type.
    int GetInputStorageTypeRowElements(InputStorageTypes storage)
    {
        switch(storage)
        {
        case InputStorageTypes::Value:
            return 1;

        case InputStorageTypes::Vector2:
            return 2;

        case InputStorageTypes::Vector3:
            return 3;

        case InputStorageTypes::Vector4:
            return 4;

        case InputStorageTypes::Matrix4x4:
            return 4;

        default:
            ASSERT(false, "Unknown input storage type!");
            return 0;
        }
    }

    // Gets the row count of an input storage type.
    int GetInputStorageTypeRowCount(InputStorageTypes storage)
    {
        switch(storage)
        {
        case InputStorageTypes::Value:
        case InputStorageTypes::Vector2:
        case InputStorageTypes::Vector3:
        case InputStorageTypes::Vector4:
            return 1;

        case InputStorageTypes::Matrix4x4:
            return 4;

        default:
            ASSERT(false, "Unknown input storage type!");
            return 0;
        }
    }

    // Gets the size of an input data type.
    int GetInputDataTypeBytes(GLint type)
    {
        switch(type)
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

    // Constant definitions.
    const GLuint InvalidHandle = 0;
}

InputAttribute::InputAttribute() :
    buffer(nullptr),
    storage(InputStorageTypes::Invalid),
    type(GL_INVALID_ENUM),
    normalize(false)
{
}

InputAttribute::InputAttribute(const Buffer* buffer, InputStorageTypes storage, GLenum type, bool normalize) :
    buffer(buffer),
    storage(storage),
    type(type),
    normalize(normalize)
{
}

InputLayoutInfo::InputLayoutInfo() :
    attributes(nullptr),
    attributeCount(0)
{
}

InputLayoutInfo::InputLayoutInfo(const InputAttribute* attributes, int attributeCount) :
    attributes(attributes), attributeCount(attributeCount)
{
}

InputLayout::InputLayout() :
    m_handle(InvalidHandle)
{
}

InputLayout::~InputLayout()
{
    this->DestroyHandle();
}

void InputLayout::DestroyHandle()
{
    // Release the vertex array handle.
    if(m_handle != InvalidHandle)
    {
        glDeleteVertexArrays(1, &m_handle);
        m_handle = InvalidHandle;
    }
}

bool InputLayout::Create(const InputLayoutInfo& info)
{
    LOG() << "Creating vertex input..." << LOG_INDENT();

    // Check if handle has been already created.
    VERIFY(m_handle == InvalidHandle, "Input layout instance has been already initialized!");

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
        const InputAttribute& attribute = info.attributes[i];

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

        if(attribute.storage == InputStorageTypes::Invalid)
        {
            LOG_ERROR() << "Invalid argument - \"attribute[" << i << "].storage\" is invalid!";
            return false;
        }

        if(attribute.type == GL_INVALID_ENUM)
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
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
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
        const InputAttribute& attribute = info.attributes[i];

        // Bind a vertex buffer.
        if(currentBuffer != attribute.buffer)
        {
            glBindBuffer(GL_ARRAY_BUFFER, attribute.buffer->GetHandle());

            currentBuffer = attribute.buffer;
            currentOffset = 0;
        }

        // Setup vertex attributes for each row of an input storage.
        for(int l = 0; l < GetInputStorageTypeRowCount(attribute.storage); ++l)
        {
            // Enable vertex attribute.
            glEnableVertexAttribArray(currentLocation);

            // Set vertex attribute pointer.
            glVertexAttribPointer(
                currentLocation,
                GetInputStorageTypeRowElements(attribute.storage),
                attribute.type,
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
            currentOffset += GetInputDataTypeBytes(attribute.type) * GetInputStorageTypeRowElements(attribute.storage);
        }
    }

    // Success!
    LOG_INFO() << "Success!";

    return initialized = true;
}

GLuint InputLayout::GetHandle() const
{
    ASSERT(m_handle != InvalidHandle, "Vertex array handle has not been created!");

    return m_handle;
}

bool InputLayout::IsValid() const
{
    return m_handle != InvalidHandle;
}
