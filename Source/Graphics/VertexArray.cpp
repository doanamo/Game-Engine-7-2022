/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#include "Graphics/Precompiled.hpp"
#include "Graphics/VertexArray.hpp"
#include "Graphics/RenderContext.hpp"
#include "Graphics/Buffer.hpp"
using namespace Graphics;

namespace
{
    int GetVertexAttributeTypeRowElements(VertexArray::AttributeType type)
    {
        switch(type)
        {
        case VertexArray::AttributeType::Value:
            return 1;

        case VertexArray::AttributeType::Vector2:
            return 2;

        case VertexArray::AttributeType::Vector3:
            return 3;

        case VertexArray::AttributeType::Vector4:
            return 4;

        case VertexArray::AttributeType::Matrix4x4:
            return 4;

        default:
            ASSERT(false, "Unknown vertex attribute type!");
            return 0;
        }
    }

    int GetVertexAttributeTypeRowCount(VertexArray::AttributeType type)
    {
        switch(type)
        {
        case VertexArray::AttributeType::Value:
        case VertexArray::AttributeType::Vector2:
        case VertexArray::AttributeType::Vector3:
        case VertexArray::AttributeType::Vector4:
            return 1;

        case VertexArray::AttributeType::Matrix4x4:
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

VertexArray::CreateResult VertexArray::Create(RenderContext* renderContext, const FromArrayParams& params)
{
    LOG("Creating vertex array...");
    LOG_SCOPED_INDENT();

    // Validate arguments.
    CHECK_ARGUMENT_OR_RETURN(renderContext != nullptr,
        Common::Failure(CreateErrors::InvalidArgument));
    CHECK_ARGUMENT_OR_RETURN(params.attributeCount > 0,
        Common::Failure(CreateErrors::InvalidArgument));
    CHECK_ARGUMENT_OR_RETURN(params.attributes != nullptr,
        Common::Failure(CreateErrors::InvalidArgument));

    for(std::size_t i = 0; i < params.attributeCount; ++i)
    {
        const Attribute& attribute = params.attributes[i];

        CHECK_ARGUMENT_OR_RETURN(attribute.buffer != nullptr,
            Common::Failure(CreateErrors::InvalidAttribute));
        CHECK_ARGUMENT_OR_RETURN(attribute.buffer->GetType() == GL_ARRAY_BUFFER,
            Common::Failure(CreateErrors::InvalidAttribute));
        CHECK_ARGUMENT_OR_RETURN(attribute.attributeType != AttributeType::Invalid,
            Common::Failure(CreateErrors::InvalidAttribute));
        CHECK_ARGUMENT_OR_RETURN(attribute.valueType != OpenGL::InvalidEnum,
            Common::Failure(CreateErrors::InvalidAttribute));
    }

    // Create instance.
    auto instance = std::unique_ptr<VertexArray>(new VertexArray());

    // Create vertex array object.
    glGenVertexArrays(1, &instance->m_handle);
    OpenGL::CheckErrors();

    if(instance->m_handle == OpenGL::InvalidHandle)
    {
        LOG_ERROR("Vertex array handle could not be created!");
        return Common::Failure(CreateErrors::FailedResourceCreation);
    }

    // Bind vertex array handle.
    glBindVertexArray(instance->m_handle);

    SCOPE_GUARD([&renderContext]
    {
        glBindVertexArray(renderContext->GetState().GetVertexArrayBinding());
        glBindBuffer(GL_ARRAY_BUFFER, renderContext->GetState().GetBufferBinding(GL_ARRAY_BUFFER));
    });

    // Setup vertex attribute array.
    const Buffer* currentBuffer = nullptr;

    int currentLocation = 0;
    int currentOffset = 0;

    for(std::size_t i = 0; i < params.attributeCount; ++i)
    {
        const Attribute& attribute = params.attributes[i];

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
                Common::NumericalCast<GLsizei>(attribute.buffer->GetElementSize()),
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
    instance->m_renderContext = renderContext;

    // Success!
    return Common::Success(std::move(instance));
}

GLuint VertexArray::GetHandle() const
{
    return m_handle;
}
