/*
    Copyright (c) 2018 Piotr Doan. All rights reserved.
*/

#pragma once

/*
    Graphics Input Layout

    Creates an input layout that binds buffers to shader inputs on the pipeline.

    void ExampleGraphicsInputLayout()
    {
        // Define the layout of input attributes.
        const Graphics::InputAttribute inputAttributes[] =
        {
            { &vertexBuffer, Graphics::InputAttributeTypes::Float2 }, // Position
            { &vertexBuffer, Graphics::InputAttributeTypes::Float2 }, // Texture
            { &vertexBuffer, Graphics::InputAttributeTypes::Float4 }, // Color
        };

        Graphics::InputLayoutInfo inputLayoutInfo;
        inputLayoutInfo.attributeCount = Utility::StaticArraySize(inputAttributes);
        inputLayoutInfo.attributes = &inputAttributes[0];
    
        // Create an input layout instance.
        Graphics::InputLayout inputLayout;
        inputLayout.Create(inputLayoutInfo);
    
        // Bind an input layout.
        glBindVertexArray(inputLayout.GetHandle());
    }
*/

namespace Graphics
{
    // Forward declarations.
    class Buffer;

    // Input storage.
    enum class InputStorageTypes
    {
        Invalid,

        Value,
        
        Vector2,
        Vector3,
        Vector4,

        Matrix4x4,

        Count,
    };

    // Input attribute structure.
    struct InputAttribute
    {
        InputAttribute();
        InputAttribute(const Buffer* buffer, InputStorageTypes storage, GLenum type, bool normalized);

        const Buffer* buffer;
        InputStorageTypes storage;
        GLenum type;
        bool normalize;
    };

    // Input layout info structure.
    struct InputLayoutInfo
    {
        InputLayoutInfo();
        InputLayoutInfo(const InputAttribute* attributes, int attributeCount);

        const InputAttribute* attributes;
        int attributeCount;
    };

    // Input layout class.
    class InputLayout
    {
    public:
        InputLayout();
        ~InputLayout();

        // Initializes the vertex input instance.
        bool Create(const InputLayoutInfo& info);

        // Gets the vertex array object handle.
        GLuint GetHandle() const;

        // Checks if instance is valid.
        bool IsValid() const;

    private:
        // Destroys the internal handle.
        void DestroyHandle();

    private:
        // Vertex array handle.
        GLuint m_handle;
    };
}
