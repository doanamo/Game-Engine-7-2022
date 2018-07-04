/*
    Copyright (c) 2018 Piotr Doan. All rights reserved.
*/

#pragma once

#include <stack>
#include "Graphics/RenderState.hpp"

namespace System
{
    class Window;
}

/*
    Graphics Render Context
*/

namespace Graphics
{
    // Render context class.
    class RenderContext
    {
    public:
        RenderContext();
        ~RenderContext();

        // Initialize the graphics context.
        bool Initialize(System::Window* window);

        // Make the context current.
        void MakeCurrent();

        // Pushes a copy of the state on the top of the stack.
        void PushState();

        // Pops the current state at the top of the stack.
        void PopState();

        // Gets the current state on top of the stack.
        RenderState& GetState();

        // Checks if the instance is valid.
        bool IsValid() const;

    private:
        // Window instance.
        System::Window* m_window;

        // Current render state.
        RenderState m_currentState;

        // Stack of render states.
        std::stack<RenderState> m_pushedStates;

        // Initialization state.
        bool m_initialized;
    };
}
