/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#pragma once

#include "Graphics/RenderState.hpp"

/*
    Vertex Array

    Creates vertex array that binds buffers to shader inputs on the pipeline.

    void ExampleGraphicsVertexArray(Graphics::RenderContext* renderContext)
    {
        // Define the layout of input attributes.
        const Graphics::VertexAttribute inputAttributes[] =
        {
            { &vertexBuffer, Graphics::VertexAttributeType::Vector3, GL_FLOAT,         false }, // Position
            { &vertexBuffer, Graphics::VertexAttributeType::Vector2, GL_FLOAT,         false }, // Texture
            { &vertexBuffer, Graphics::VertexAttributeType::Vector4, GL_UNSIGNED_BYTE, true  }, // Color
        };

        Graphics::VertexArrayInfo vertexArrayInfo;
        vertexArrayInfo.attributeCount = Utility::StaticArraySize(inputAttributes);
        vertexArrayInfo.attributes = &inputAttributes[0];
    
        // Create vertex array instance.
        Graphics::VertexArray vertexArray();
        vertexArray.Initialize(renderContext, vertexArrayInfo);
    
        // Bind vertex array.
        glBindVertexArray(vertexArray.GetHandle());
    }
*/

namespace Graphics
{
    class RenderContext;
    class Buffer;

    enum class VertexAttributeType
    {
        Invalid,

        Value,
        Vector2,
        Vector3,
        Vector4,
        Matrix4x4,

        Count,
    };

    struct VertexAttribute
    {
        const Buffer* buffer = nullptr;
        VertexAttributeType attributeType = VertexAttributeType::Invalid;
        GLenum valueType = OpenGL::InvalidEnum;
        bool normalize = false;
    };

    struct VertexArrayInfo
    {
        const VertexAttribute* attributes = nullptr;
        std::size_t attributeCount = 0;
    };

    class VertexArray final : private NonCopyable, public Resettable<VertexArray>
    {
    public:
        enum class InitializeErrors
        {
            InvalidArgument,
            InvalidAttribute,
            FailedResourceCreation,
        };

        using InitializeResult = Result<void, InitializeErrors>;

    public:
        VertexArray();
        ~VertexArray();

        InitializeResult Initialize(RenderContext* renderContext, const VertexArrayInfo& info);

        GLuint GetHandle() const;
        bool IsInitialized() const;

    private:
        RenderContext* m_renderContext = nullptr;
        GLuint m_handle = OpenGL::InvalidHandle;
        bool m_initialized = false;
    };
}
