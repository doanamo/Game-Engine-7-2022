/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#include "Graphics/VertexArray.hpp"
#include "Graphics/RenderContext.hpp"
#include "Graphics/Buffer.hpp"
using namespace Graphics;

namespace
{
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

VertexArray::VertexArray() = default;

VertexArray::~VertexArray()
{
    if(m_handle != OpenGL::InvalidHandle)
    {
        glDeleteVertexArrays(1, &m_handle);
        OpenGL::CheckErrors();
    }
}

VertexArray::InitializeResult VertexArray::Initialize(RenderContext* renderContext, const VertexArrayInfo& info)
{
    LOG("Creating vertex array...");
    LOG_SCOPED_INDENT();

    // Setup initialization guard.
    VERIFY(!m_initialized, "Instance has already been initialized!");
    SCOPE_GUARD_IF(!m_initialized, this->Reset());

    // Validate arguments.
    CHECK_ARGUMENT_OR_RETURN(renderContext != nullptr, Failure(InitializeErrors::InvalidArgument));
    CHECK_ARGUMENT_OR_RETURN(info.attributeCount > 0, Failure(InitializeErrors::InvalidArgument));
    CHECK_ARGUMENT_OR_RETURN(info.attributes != nullptr, Failure(InitializeErrors::InvalidArgument));

    for(std::size_t i = 0; i < info.attributeCount; ++i)
    {
        const VertexAttribute& attribute = info.attributes[i];

        CHECK_ARGUMENT_OR_RETURN(attribute.buffer != nullptr, Failure(InitializeErrors::InvalidAttribute));
        CHECK_ARGUMENT_OR_RETURN(attribute.buffer->IsInitialized(), Failure(InitializeErrors::InvalidAttribute));
        CHECK_ARGUMENT_OR_RETURN(attribute.buffer->GetType() == GL_ARRAY_BUFFER, Failure(InitializeErrors::InvalidAttribute));
        CHECK_ARGUMENT_OR_RETURN(attribute.attributeType != VertexAttributeType::Invalid, Failure(InitializeErrors::InvalidAttribute));
        CHECK_ARGUMENT_OR_RETURN(attribute.valueType != OpenGL::InvalidEnum, Failure(InitializeErrors::InvalidAttribute));
    }

    // Create vertex array object.
    glGenVertexArrays(1, &m_handle);
    OpenGL::CheckErrors();

    if(m_handle == OpenGL::InvalidHandle)
    {
        LOG_ERROR("Vertex array handle could not be created!");
        return Failure(InitializeErrors::FailedResourceCreation);
    }

    // Bind vertex array handle.
    glBindVertexArray(m_handle);

    SCOPE_GUARD_BEGIN();
    {
        glBindVertexArray(renderContext->GetState().GetVertexArrayBinding());
        glBindBuffer(GL_ARRAY_BUFFER, renderContext->GetState().GetBufferBinding(GL_ARRAY_BUFFER));
    }
    SCOPE_GUARD_END();

    // Setup vertex attribute array.
    const Buffer* currentBuffer = nullptr;

    int currentLocation = 0;
    int currentOffset = 0;

    for(std::size_t i = 0; i < info.attributeCount; ++i)
    {
        const VertexAttribute& attribute = info.attributes[i];

        // Bind a vertex buffer.
        if(currentBuffer != attribute.buffer)
        {
            glBindBuffer(GL_ARRAY_BUFFER, attribute.buffer->GetHandle());
            OpenGL::CheckErrors();

            currentBuffer = attribute.buffer;
            currentOffset = 0;
        }

        // Setup vertex attributes for each row of an input storage.
        for(int l = 0; l < GetVertexAttributeTypeRowCount(attribute.attributeType); ++l)
        {
            // Enable vertex attribute.
            glEnableVertexAttribArray(currentLocation);
            OpenGL::CheckErrors();

            // Set vertex attribute pointer.
            glVertexAttribPointer(
                currentLocation,
                GetVertexAttributeTypeRowElements(attribute.attributeType),
                attribute.valueType,
                attribute.normalize ? GL_TRUE : GL_FALSE,
                Utility::NumericalCast<GLsizei>(attribute.buffer->GetElementSize()),
                (void*)(intptr_t)currentOffset
            );

            OpenGL::CheckErrors();

            // Make input location instanced.
            if(attribute.buffer->IsInstanced())
            {
                glVertexAttribDivisor(currentLocation, 1);
                OpenGL::CheckErrors();
            }

            // Increment current location.
            currentLocation += 1;

            // Increment current offset.
            currentOffset += GetVertexAttributeValueBytes(attribute.valueType) * GetVertexAttributeTypeRowElements(attribute.attributeType);
        }
    }

    // Save render context reference.
    m_renderContext = renderContext;

    // Success!
    m_initialized = true;
    return Success();
}

GLuint VertexArray::GetHandle() const
{
    ASSERT(m_initialized, "Vertex array has not been initialized!");
    return m_handle;
}

bool VertexArray::IsInitialized() const
{
    return m_initialized;
}
