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

    Manages internal state of rendering system.
*/

namespace Graphics
{
    class RenderContext final : private NonCopyable, public Resettable<RenderContext>
    {
    public:
        enum class InitializeErrors
        {
            InvalidArgument,
            FailedStateInitialization,
        };

        using InitializeResult = Result<void, InitializeErrors>;

    public:
        RenderContext();
        ~RenderContext();

        InitializeResult Initialize(System::Window* window);
        void MakeCurrent();

        RenderState& PushState();
        void PopState();

        RenderState& GetState();
        bool IsInitialized() const;

    private:
        System::Window* m_window = nullptr;

        RenderState m_currentState;
        std::stack<RenderState> m_pushedStates;

        bool m_initialized = false;
    };
}
