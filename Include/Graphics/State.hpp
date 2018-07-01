/*
    Copyright (c) 2018 Piotr Doan. All rights reserved.
*/

#pragma once

/*
    Graphics State
*/

namespace Graphics
{
    // OpenGL API enumerations and values.
    namespace OpenGL
    {
        // List of buffer binding targets.
        static const GLenum BufferBindingTargets[] =
        {
            GL_INVALID_ENUM,
            GL_ARRAY_BUFFER,
            GL_ATOMIC_COUNTER_BUFFER,
            GL_COPY_READ_BUFFER,
            GL_COPY_WRITE_BUFFER,
            GL_DISPATCH_INDIRECT_BUFFER,
            GL_DRAW_INDIRECT_BUFFER,
            GL_ELEMENT_ARRAY_BUFFER,
            GL_PIXEL_PACK_BUFFER,
            GL_PIXEL_UNPACK_BUFFER,
            GL_QUERY_BUFFER,
            GL_SHADER_STORAGE_BUFFER,
            GL_TEXTURE_BUFFER,
            GL_TRANSFORM_FEEDBACK_BUFFER,
            GL_UNIFORM_BUFFER,
        };

        const int BufferBindingTargetCount = Utility::StaticArraySize(OpenGL::BufferBindingTargets);
    }

    // State structure.
    struct State
    {
        State();

        GLint bindBuffer[OpenGL::BufferBindingTargetCount];
        GLint GetBindBuffer(GLenum target) const;
    };
}
