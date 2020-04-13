/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#pragma once

#include <stack>
#include "Graphics/RenderState.hpp"

namespace System
{
    class Window;
}

/*
    Render Context

    Manages the internal state of a rendering system.
*/

namespace Graphics
{
    class RenderContext : private NonCopyable
    {
    public:
        RenderContext() = default;
        ~RenderContext() = default;

        RenderContext(RenderContext&& other);
        RenderContext& operator=(RenderContext&& other);

        bool Initialize(System::Window* window);
        void MakeCurrent();

        RenderState& PushState();
        void PopState();

        RenderState& GetState();
        bool IsValid() const;

    private:
        System::Window* m_window = nullptr;

        RenderState m_currentState;
        std::stack<RenderState> m_pushedStates;

        bool m_initialized = false;
    };
}
