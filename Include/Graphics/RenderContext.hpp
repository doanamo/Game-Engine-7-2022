/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#pragma once

#include <stack>
#include <Core/Service.hpp>
#include "Graphics/RenderState.hpp"

namespace Core
{
    class ServiceStorage;
}

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
    class RenderContext final : public Core::Service
    {
        REFLECTION_ENABLE(RenderContext, Core::Service)

    public:
        struct CreateParams
        {
            const Core::ServiceStorage* services = nullptr;
        };

        enum class CreateErrors
        {
            InvalidArgument,
            FailedStateCreation,
        };

        using CreateResult = Common::Result<std::unique_ptr<RenderContext>, CreateErrors>;
        static CreateResult Create(const CreateParams& params);

    public:
        ~RenderContext() override;

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

REFLECTION_TYPE(Graphics::RenderContext, Core::Service)
