/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#pragma once

#include "Graphics/RenderState.hpp"

/*
    Buffer
    
    Generic buffer base class that can handle different types of OpenGL buffers.
    Supported buffer types include vertex buffer, index buffer and instance buffer.
    
    void ExampleGraphicsBuffer(Graphics::RenderContext* renderContext)
    {
        // Define vertex structure.
        struct Vertex
        {
            glm::vec3 position;
            glm::vec4 color;
        };
        
        // Define vertex geometry.
        const Vertex vertices[] =
        {
            { glm::vec3( 0.0f,  0.433f,  0.0f), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f) }, // Top
            { glm::vec3( 0.5f, -0.433f,  0.0f), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f) }, // Right
            { glm::vec3(-0.5f, -0.433f,  0.0f), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f) }, // Left
        };

        // Describe buffer info.
        Graphics::BufferInfo bufferInfo;
        bufferInfo.elementSize = sizeof(Vertex);
        bufferInfo.elementCount = Utility::StaticArraySize(vertices);
        bufferInfo.data = &vertices[0];
        
        // Create a vertex buffer.
        Graphics::VertexBuffer vertexBuffer();
        vertexBuffer.Initialize(renderContext, bufferInfo);

        // Retrieve an OpenGL handle.
        GLuint handle = vertexBuffer.GetHandle();
    }
*/

namespace Graphics
{
    class RenderContext;

    struct BufferInfo
    {
        GLenum usage = GL_STATIC_DRAW;
        std::size_t elementSize = 0;
        std::size_t elementCount = 0;
        const void* data = nullptr;
    };

    class Buffer : private NonCopyable
    {
    protected:
        Buffer(GLenum type);
        virtual ~Buffer();

        Buffer(Buffer&& other);
        Buffer& operator=(Buffer&& other);

    public:
        bool Initialize(RenderContext* renderContext, const BufferInfo& info);
        void Update(const void* data, std::size_t elementCount);

        GLenum GetType() const;
        virtual const char* GetName() const = 0;
        GLuint GetHandle() const;
        std::size_t GetElementSize() const;
        std::size_t GetElementCount() const;
        virtual GLenum GetElementType() const;
        bool IsValid() const;
        virtual bool IsInstanced() const;

    private:
        void DestroyHandle();

    protected:
        RenderContext* m_renderContext = nullptr;

        GLenum m_type;
        GLenum m_usage = OpenGL::InvalidEnum;
        GLuint m_handle = OpenGL::InvalidHandle;

        std::size_t m_elementSize = 0;
        std::size_t m_elementCount = 0;
    };
}

/*
    Vertex Buffer
*/

namespace Graphics
{
    class VertexBuffer : public Buffer
    {
    public:
        VertexBuffer();

        VertexBuffer(VertexBuffer&& other);
        VertexBuffer& operator=(VertexBuffer&& other);

        const char* GetName() const override;
    };
}

/*
    Index Buffer
*/

namespace Graphics
{
    class IndexBuffer : public Buffer
    {
    public:
        IndexBuffer();

        IndexBuffer(IndexBuffer&& other);
        IndexBuffer& operator=(IndexBuffer&& other);

        GLenum GetElementType() const override;
        const char* GetName() const override;
    };
}

/*
    Graphics Instance Buffer
*/

namespace Graphics
{
    class InstanceBuffer : public Buffer
    {
    public:
        InstanceBuffer();

        InstanceBuffer(InstanceBuffer&& other);
        InstanceBuffer& operator=(InstanceBuffer&& other);

        const char* GetName() const override;
        bool IsInstanced() const override;
    };
}
