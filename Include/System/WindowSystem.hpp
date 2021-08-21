/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#pragma once

#include <Core/EngineSystem.hpp>

/*
    Window System

    Wrapper for window to be used as engine system.
*/

namespace System
{
    class Window;

    class WindowSystem final : public Core::EngineSystem
    {
        REFLECTION_ENABLE(WindowSystem, Core::EngineSystem)

    public:
        WindowSystem();
        ~WindowSystem() override;

        Window& GetWindow()
        {
            ASSERT(m_window);
            return *m_window;
        }

    private:
        bool OnAttach(const Core::EngineSystemStorage& engineSystems) override;
        void OnRunEngine() override;
        void OnBeginFrame() override;
        void OnEndFrame() override;
        bool IsRequestingExit() override;

    private:
        std::unique_ptr<Window> m_window;
    };
}

REFLECTION_TYPE(System::WindowSystem, Core::EngineSystem)
