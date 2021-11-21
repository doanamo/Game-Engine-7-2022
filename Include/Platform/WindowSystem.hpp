/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#pragma once

#include "Platform/WindowEvents.hpp"
#include <Common/Event/EventReceiver.hpp>
#include <Core/System/EngineSystem.hpp>

namespace Core
{
    class FrameRateLimiter;
}

/*
    Window System

    Wrapper for window to be used as engine system.
*/

namespace Platform
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

        void OnWindowFocusChange(const WindowEvents::Focus& event);

    private:
        std::unique_ptr<Window> m_window;
        Core::FrameRateLimiter* m_frameRateLimiter;

        struct Receivers
        {
            Event::Receiver<void(const WindowEvents::Focus&)> focusChange;
        } m_receivers;
    };
}

REFLECTION_TYPE(Platform::WindowSystem, Core::EngineSystem)
