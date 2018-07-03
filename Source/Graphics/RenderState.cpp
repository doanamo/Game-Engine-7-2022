/*
    Copyright (c) 2018 Piotr Doan. All rights reserved.
*/

#include "Precompiled.hpp"
#include "Graphics/RenderState.hpp"
using namespace Graphics;

void OpenGL::CheckErrors()
{
    #ifndef NDEBUG
        GLenum error;
        while((error = glGetError()) != GL_NO_ERROR)
        {
            std::stringstream stream;
            stream << std::hex << std::setfill('0') << std::setw(4) << error;

            LOG_WARNING() << "Encountered OpenGL error with code 0x" << stream.str() << "!";
            ASSERT(error != GL_NO_ERROR, "Breaking due to encountered OpenGL error!");
        }
    #endif
}

RenderState::RenderState()
{
    // glBindVertexArray
    glGetIntegerv(GL_VERTEX_ARRAY_BINDING, (GLint*)&m_vertexArrayBinding);
    OpenGL::CheckErrors();

    // glBindBuffer
    m_bufferBinding[0] = OpenGL::InvalidHandle;

    for(int i = 1; i < OpenGL::BufferBindingTargetCount; ++i)
    {
        glGetIntegerv(std::get<1>(OpenGL::BufferBindingTargets[i]), (GLint*)&m_bufferBinding[i]);
        OpenGL::CheckErrors();
    }

    // glBindTexture
    m_textureBinding[0] = OpenGL::InvalidHandle;

    for(int i = 1; i < OpenGL::TextureBindingTargetCount; ++i)
    {
        glGetIntegerv(std::get<1>(OpenGL::TextureBindingTargets[i]), (GLint*)&m_textureBinding[i]);
        OpenGL::CheckErrors();
    }

    // glPixelStore
    m_pixelStore[0] = 0;

    for(int i = 1; i < OpenGL::PixelStoreParameterCount; ++i)
    {
        glGetIntegerv(OpenGL::PixelStoreParameters[i], &m_pixelStore[i]);
        OpenGL::CheckErrors();
    }
}

void RenderState::BindVertexArray(GLuint array)
{
    // Check if states match.
    if(GetVertexArrayBinding() == array)
        return;

    // Call OpenGL function.
    glBindVertexArray(array);
    OpenGL::CheckErrors();

    // Save changed state.
    m_vertexArrayBinding = array;
}

GLuint RenderState::GetVertexArrayBinding() const
{
    return m_vertexArrayBinding;
}

void RenderState::BindBuffer(GLenum target, GLuint buffer)
{
    ASSERT(target != GL_INVALID_ENUM, "Unsupported buffer binding target!");

    // Check if states match.
    if(GetBufferBinding(target) == buffer)
        return;

    // Call OpenGL function.
    glBindBuffer(target, buffer);
    OpenGL::CheckErrors();

    // Save changed state.
    for(int i = 1; i < OpenGL::BufferBindingTargetCount; ++i)
    {
        if(std::get<0>(OpenGL::BufferBindingTargets[i]) == target)
            m_bufferBinding[i] = buffer;
    }
}

GLuint RenderState::GetBufferBinding(GLenum target) const
{
    ASSERT(target != GL_INVALID_ENUM, "Unsupported buffer binding target!");

    for(int i = 1; i < OpenGL::BufferBindingTargetCount; ++i)
    {
        if(std::get<0>(OpenGL::BufferBindingTargets[i]) == target)
            return m_bufferBinding[i];
    }

    return m_bufferBinding[0];
}

void RenderState::BindTexture(GLenum target, GLuint texture)
{
    ASSERT(target != GL_INVALID_ENUM, "Unsupported texture binding target!");

    // Check if states match.
    if(GetTextureBinding(target) == texture)
        return;

    // Call OpenGL function.
    glBindTexture(target, texture);
    OpenGL::CheckErrors();

    // Save changed state.
    for(int i = 1; i < OpenGL::TextureBindingTargetCount; ++i)
    {
        if(std::get<0>(OpenGL::TextureBindingTargets[i]) == target)
            m_textureBinding[i] = texture;
    }
}

GLuint RenderState::GetTextureBinding(GLenum target) const
{
    ASSERT(target != GL_INVALID_ENUM, "Unsupported texture binding target!");

    for(int i = 1; i < OpenGL::TextureBindingTargetCount; ++i)
    {
        if(std::get<0>(OpenGL::TextureBindingTargets[i]) == target)
            return m_textureBinding[i];
    }

    return m_textureBinding[0];
}

void RenderState::PixelStore(GLenum pname, GLint param)
{
    ASSERT(pname != GL_INVALID_ENUM, "Unsupported pixel store parameter!");

    // Check if states match.
    if(GetTextureBinding(pname) == param)
        return;

    // Call OpenGL function.
    glPixelStorei(pname, param);
    OpenGL::CheckErrors();

    // Save changed state.
    for(int i = 1; i < OpenGL::PixelStoreParameterCount; ++i)
    {
        if(OpenGL::PixelStoreParameters[i] == pname)
            m_pixelStore[i] = param;
    }
}

GLint RenderState::GetPixelStore(GLenum pname) const
{
    ASSERT(pname != GL_INVALID_ENUM, "Unsupported pixel store parameter!");

    for(int i = 1; i < OpenGL::PixelStoreParameterCount; ++i)
    {
        if(OpenGL::PixelStoreParameters[i] == pname)
            return m_pixelStore[i];
    }

    return m_pixelStore[0];
}
