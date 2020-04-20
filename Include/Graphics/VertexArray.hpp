/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#pragma once

#include "Graphics/RenderState.hpp"

/*
    Vertex Array

    Creates vertex array that binds buffers to shader inputs on the pipeline.
*/

namespace Graphics
{
    class RenderContext;
    class Buffer;

    class VertexArray final : private Common::NonCopyable
    {
    public:
        enum class AttributeType
        {
            Invalid,

            Value,
            Vector2,
            Vector3,
            Vector4,
            Matrix4x4,

            Count,
        };

        struct Attribute
        {
            const Buffer* buffer = nullptr;
            AttributeType attributeType = AttributeType::Invalid;
            GLenum valueType = OpenGL::InvalidEnum;
            bool normalize = false;
        };

        struct FromArrayParams
        {
            const Attribute* attributes = nullptr;
            std::size_t attributeCount = 0;
        };

        enum class CreateErrors
        {
            InvalidArgument,
            InvalidAttribute,
            FailedResourceCreation,
        };

        using CreateResult = Common::Result<std::unique_ptr<VertexArray>, CreateErrors>;
        static CreateResult Create(RenderContext* renderContext, const FromArrayParams& params);

    public:
        ~VertexArray();

        GLuint GetHandle() const;

    private:
        VertexArray();

    private:
        RenderContext* m_renderContext = nullptr;
        GLuint m_handle = OpenGL::InvalidHandle;
    };
}
