/*
    Copyright (c) 2018 Piotr Doan. All rights reserved.
*/

#pragma once

/*
    Graphics Buffer
    
    Generic buffer base class that can handle different types of OpenGL buffers.
    Supported buffer types include vertex buffer, index buffer and instance buffer.
    
    void ExampleGraphicsBuffer()
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
        Graphics::VertexBuffer vertexBuffer;
        vertexBuffer.Create(bufferInfo);

        // Retrieve an OpenGL handle.
        GLuint handle = vertexBuffer.GetHandle();
    }
*/

namespace Graphics
{
    // Buffer info structure.
    struct BufferInfo
    {
        BufferInfo();

        GLenum usage;
        unsigned int elementSize;
        unsigned int elementCount;
        const void* data;
    };

    // Buffer class.
    class Buffer
    {
    protected:
        Buffer(GLenum type);
        virtual ~Buffer();

    public:
        // Initializes the buffer instance.
        bool Create(const BufferInfo& info);

        // Updates the buffer's data.
        void Update(const void* data, int elementCount);

        // Gets the buffer's type.
        GLenum GetType() const;

        // Gets the buffer's name.
        virtual const char* GetName() const = 0;

        // Gets the buffer's handle.
        GLuint GetHandle() const;

        // Gets the buffer's element size.
        unsigned int GetElementSize() const;

        // Gets the buffer's element count.
        unsigned int GetElementCount() const;

        // Gets the buffer's element type.
        virtual GLenum GetElementType() const;

        // Checks if the buffer is valid.
        bool IsValid() const;

        // Checks if the buffer is instanced.
        virtual bool IsInstanced() const;

    private:
        // Destroys the internal handle.
        void DestroyHandle();

    protected:
        // Buffer handle.
        GLenum m_type;
        GLenum m_usage;
        GLuint m_handle;

        // Buffer parameters.
        unsigned int m_elementSize;
        unsigned int m_elementCount;
    };
}

/*
    Graphics Vertex Buffer
*/

namespace Graphics
{
    class VertexBuffer : public Buffer
    {
    public:
        VertexBuffer();

        // Returns the buffer's name.
        const char* GetName() const override;
    };
}

/*
    Graphics Index Buffer
*/

namespace Graphics
{
    class IndexBuffer : public Buffer
    {
    public:
        IndexBuffer();

        // Gets the type of index indices.
        GLenum GetElementType() const override;

        // Returns the buffer's name.
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

        // Returns the buffer's name.
        const char* GetName() const override;

        // Returns true for this type of a buffer.
        bool IsInstanced() const override;
    };
}
