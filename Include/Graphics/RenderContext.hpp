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
    class RenderContext final : private NonCopyable
    {
    public:
        enum class CreateErrors
        {
            InvalidArgument,
            FailedStateCreation,
        };

        using CreateResult = Result<std::unique_ptr<RenderContext>, CreateErrors>;
        static CreateResult Create(System::Window* window);

    public:
        ~RenderContext();

        void MakeCurrent();
        RenderState& PushState();
        RenderState& GetState();
        void PopState();

    private:
        RenderContext();

    private:
        System::Window* m_window = nullptr;

        RenderState m_currentState;
        std::stack<RenderState> m_pushedStates;
    };
}
