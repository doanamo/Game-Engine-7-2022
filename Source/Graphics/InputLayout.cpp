/*
    Copyright (c) 2018 Piotr Doan. All rights reserved.
*/

#include "Precompiled.hpp"
#include "Graphics/InputLayout.hpp"
#include "Graphics/Buffer.hpp"
using namespace Graphics;

namespace
{
    // Gets the row size of a input attribute type.
    int GetInputAttributeTypeRowSize(InputAttributeTypes type)
    {
        switch(type)
        {
        case InputAttributeTypes::Float1:
            return 1;

        case InputAttributeTypes::Float2:
            return 2;

        case InputAttributeTypes::Float3:
            return 3;

        case InputAttributeTypes::Float4:
            return 4;

        case InputAttributeTypes::Float4x4:
            return 4;

        default:
            ASSERT(false, "Unknown attribute type!");
            return 0;
        }
    }

    // Gets the row count of a input attribute type.
    int GetInputAttributeTypeRowCount(InputAttributeTypes type)
    {
        switch(type)
        {
        case InputAttributeTypes::Float1:
        case InputAttributeTypes::Float2:
        case InputAttributeTypes::Float3:
        case InputAttributeTypes::Float4:
            return 1;

        case InputAttributeTypes::Float4x4:
            return 4;

        default:
            ASSERT(false, "Unknown attribute type!");
            return 0;
        }
    }

    // Gets the row offset of a input attribute type.
    int GetInputAttributeTypeRowOffset(InputAttributeTypes type)
    {
        switch(type)
        {
        case InputAttributeTypes::Float1:
            return sizeof(float) * 1;

        case InputAttributeTypes::Float2:
            return sizeof(float) * 2;

        case InputAttributeTypes::Float3:
            return sizeof(float) * 3;

        case InputAttributeTypes::Float4:
            return sizeof(float) * 4;

        case InputAttributeTypes::Float4x4:
            return sizeof(float) * 4;

        default:
            ASSERT(false, "Unknown attribute type!");
            return 0;
        }
    }

    // Gets OpenGL type enum of a input attribute type.
    GLenum GetInputAttributeTypeEnum(InputAttributeTypes type)
    {
        switch(type)
        {
        case InputAttributeTypes::Float1:
        case InputAttributeTypes::Float2:
        case InputAttributeTypes::Float3:
        case InputAttributeTypes::Float4:
        case InputAttributeTypes::Float4x4:
            return GL_FLOAT;

        default:
            ASSERT(false, "Unknown attribute type!");
            return GL_INVALID_ENUM;
        }
    }

    // Constant definitions.
    const GLuint InvalidHandle = 0;
}

InputAttribute::InputAttribute() :
    buffer(nullptr), type(InputAttributeTypes::Invalid)
{
}

InputAttribute::InputAttribute(const Buffer* buffer, InputAttributeTypes type) :
    buffer(buffer), type(type)
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

        if(attribute.type == InputAttributeTypes::Invalid)
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

        // Setup vertex attributes for each row of an input attribute.
        for(int l = 0; l < GetInputAttributeTypeRowCount(attribute.type); ++l)
        {
            // Enable vertex attribute.
            glEnableVertexAttribArray(currentLocation);

            // Set vertex attribute pointer.
            glVertexAttribPointer(
                currentLocation,
                GetInputAttributeTypeRowSize(attribute.type),
                GetInputAttributeTypeEnum(attribute.type),
                GL_FALSE,
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
            currentOffset += GetInputAttributeTypeRowOffset(attribute.type);
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
