/*
    Copyright (c) 2018 Piotr Doan. All rights reserved.
*/

#include "Precompiled.hpp"
#include "Graphics/State.hpp"
using namespace Graphics;

State::State()
{
    // glBindVertexArray
    m_bindVertexArray = OpenGL::InvalidHandle;

    // glBindBuffer
    for(int i = 0; i < OpenGL::BufferBindingTargetCount; ++i)
    {
        m_bindBuffer[i] = OpenGL::InvalidHandle;
    }
}

GLint State::GetBindVertexArray() const
{
    return m_bindVertexArray;
}

int State::GetBindBuffer(GLenum target) const
{
    for(int i = 1; i < OpenGL::BufferBindingTargetCount; ++i)
    {
        if(OpenGL::BufferBindingTargets[i] == target)
            return m_bindBuffer[i];
    }

    VERIFY(false, "Unknown or unsupported buffer binding target has been provided!");
    return 0;
}
