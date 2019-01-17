/*
    Copyright (c) 2018-2019 Piotr Doan. All rights reserved.
*/

#pragma once

/*
    Graphics Vertex Array

    Creates a vertex array that binds buffers to shader inputs on the pipeline.

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
    
        // Create a vertex array instance.
        Graphics::VertexArray vertexArray();
        vertexArray.Initialize(renderContext, vertexArrayInfo);
    
        // Bind a vertex array.
        glBindVertexArray(vertexArray.GetHandle());
    }
*/

namespace Graphics
{
    // Forward declarations.
    class RenderContext;
    class Buffer;

    // Vertex attribute type.
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

    // Vertex attribute structure.
    struct VertexAttribute
    {
        VertexAttribute();
        VertexAttribute(const Buffer* buffer, VertexAttributeType attributeType, GLenum valueType, bool normalized);

        const Buffer* buffer;
        VertexAttributeType attributeType;
        GLenum valueType;
        bool normalize;
    };

    // Vertex array info structure.
    struct VertexArrayInfo
    {
        VertexArrayInfo();
        VertexArrayInfo(const VertexAttribute* attributes, int attributeCount);

        const VertexAttribute* attributes;
        std::size_t attributeCount;
    };

    // Vertex array class.
    class VertexArray
    {
    public:
        VertexArray();
        ~VertexArray();

        // Disallow copying.
        VertexArray(const VertexArray& other) = delete;
        VertexArray& operator=(const VertexArray& other) = delete;

        // Move constructor and assignment.
        VertexArray(VertexArray&& other);
        VertexArray& operator=(VertexArray&& other);

        // Initializes the vertex array instance.
        bool Initialize(RenderContext* renderContext, const VertexArrayInfo& info);

        // Gets the vertex array object handle.
        GLuint GetHandle() const;

        // Checks if instance is valid.
        bool IsValid() const;

    private:
        // Destroys the internal handle.
        void DestroyHandle();

    private:
        // Render context.
        RenderContext* m_renderContext;

        // Vertex array handle.
        GLuint m_handle;
    };
}
