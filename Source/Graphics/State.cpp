/*
    Copyright (c) 2018 Piotr Doan. All rights reserved.
*/

#include "Precompiled.hpp"
#include "Graphics/State.hpp"
using namespace Graphics;

State::State()
{
    // Bound buffers (glBindBuffer).
    for(int i = 0; i < OpenGL::BufferBindingTargetCount; ++i)
    {
        bindBuffer[i] = 0;
    }
}

int State::GetBindBuffer(GLenum target) const
{
    for(int i = 1; i < OpenGL::BufferBindingTargetCount; ++i)
    {
        if(OpenGL::BufferBindingTargets[i] == target)
            return bindBuffer[i];
    }

    VERIFY(false, "Unknown or unsupported buffer binding target has been provided!");
    return 0;
}
