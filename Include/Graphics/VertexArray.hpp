/*
    Copyright (c) 2018 Piotr Doan. All rights reserved.
*/

#pragma once

/*
    Graphics Vertex Array

    Creates a vertex array that binds buffers to shader inputs on the pipeline.

    void ExampleGraphicsVertexArray(Graphics::RenderContext* graphics)
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
        Graphics::VertexArray vertexArray(&graphics);
        vertexArray.Create(vertexArrayInfo);
    
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
        VertexAttribute(const Buffer* buffer, VertexAttributeType type, GLenum value, bool normalized);

        const Buffer* buffer;
        VertexAttributeType type;
        GLenum value;
        bool normalize;
    };

    // Vertex array info structure.
    struct VertexArrayInfo
    {
        VertexArrayInfo();
        VertexArrayInfo(const VertexAttribute* attributes, int attributeCount);

        const VertexAttribute* attributes;
        int attributeCount;
    };

    // Vertex array class.
    class VertexArray
    {
    public:
        VertexArray(RenderContext* context);
        ~VertexArray();

        // Initializes the vertex array instance.
        bool Create(const VertexArrayInfo& info);

        // Gets the vertex array object handle.
        GLuint GetHandle() const;

        // Checks if instance is valid.
        bool IsValid() const;

    private:
        // Destroys the internal handle.
        void DestroyHandle();

    private:
        // Graphics context.
        RenderContext* m_context;

        // Vertex array handle.
        GLuint m_handle;
    };
}
