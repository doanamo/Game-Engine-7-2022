/*
    Copyright (c) 2018 Piotr Doan. All rights reserved.
*/

#pragma once

#include "Graphics/State.hpp"

namespace System
{
    class Window;
}

/*
    Graphics Context
*/

namespace Graphics
{
    // Context class.
    class Context
    {
    public:
        Context();
        ~Context();

        // Initialize the graphics context.
        bool Initialize(System::Window* window);

        // Make the context current.
        void MakeCurrent();

        // Pushes a copy of the state on the top of the stack.
        void PushState();

        // Pops the current state at the top of the stack.
        void PopState();

        // Gets the current state on top of the stack.
        const State& GetState() const;

        // Checks if the instance is valid.
        bool IsValid() const;

    private:
        // Window instance.
        System::Window* m_window;

        // Stack of states.
        std::vector<State> m_states;

        // Initialization state.
        bool m_initialized;
    };
}
