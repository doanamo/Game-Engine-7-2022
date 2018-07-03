/*
    Copyright (c) 2018 Piotr Doan. All rights reserved.
*/

#include "Precompiled.hpp"
#include "Graphics/State.hpp"
using namespace Graphics;

State::State()
{
    // glBindVertexArray
    glGetIntegerv(GL_VERTEX_ARRAY_BINDING, (GLint*)&m_bindVertexArray);

    // glBindBuffer
    m_bindBuffer[0] = OpenGL::InvalidHandle;

    for(int i = 1; i < OpenGL::BufferBindingTargetCount; ++i)
    {
        glGetIntegerv(std::get<1>(OpenGL::BufferBindingTargets[i]), (GLint*)&m_bindBuffer[i]);
        ASSERT(glGetError() == GL_NO_ERROR);
    }

    // glBindTexture
    m_bindTexture[0] = OpenGL::InvalidHandle;

    for(int i = 1; i < OpenGL::TextureBindingTargetCount; ++i)
    {
        glGetIntegerv(std::get<1>(OpenGL::TextureBindingTargets[i]), (GLint*)&m_bindTexture[i]);
        ASSERT(glGetError() == GL_NO_ERROR);

    }

    // glPixelStore
    m_pixelStore[0] = 0;

    for(int i = 1; i < OpenGL::PixelStoreParameterCount; ++i)
    {
        glGetIntegerv(OpenGL::PixelStoreParameters[i], &m_pixelStore[i]);
        ASSERT(glGetError() == GL_NO_ERROR);

    }
}

GLuint State::GetBindVertexArray() const
{
    return m_bindVertexArray;
}

GLuint State::GetBindBuffer(GLenum target) const
{
    VERIFY(target != GL_INVALID_ENUM, "Unsupported buffer binding target!");

    for(int i = 1; i < OpenGL::BufferBindingTargetCount; ++i)
    {
        if(std::get<0>(OpenGL::BufferBindingTargets[i]) == target)
            return m_bindBuffer[i];
    }

    return m_bindBuffer[0];
}

GLuint State::GetBindTexture(GLenum target) const
{
    VERIFY(target != GL_INVALID_ENUM, "Unsupported texture binding target!");

    for(int i = 1; i < OpenGL::TextureBindingTargetCount; ++i)
    {
        if(std::get<0>(OpenGL::TextureBindingTargets[i]) == target)
            return m_bindTexture[i];
    }

    return m_bindTexture[0];
}

GLint State::GetPixelStore(GLenum parameter) const
{
    VERIFY(parameter != GL_INVALID_ENUM, "Unsupported pixel store parameter!");

    for(int i = 1; i < OpenGL::PixelStoreParameterCount; ++i)
    {
        if(OpenGL::PixelStoreParameters[i] == parameter)
            return m_pixelStore[i];
    }

    return m_pixelStore[0];
}
