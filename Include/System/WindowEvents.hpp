/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#pragma once

#include <Common/Event/EventBase.hpp>

/*
    Window Events
*/

namespace System
{
    struct WindowEvents
    {
        struct Move : public Event::EventBase
        {
            REFLECTION_ENABLE(Move, Event::EventBase)

            int x;
            int y;
        };

        struct Resize : public Event::EventBase
        {
            REFLECTION_ENABLE(Resize, Event::EventBase)

            int width;
            int height;
        };

        struct Focus : public Event::EventBase
        {
            REFLECTION_ENABLE(Focus, Event::EventBase)

            bool focused;
        };

        struct Close : public Event::EventBase
        {
            REFLECTION_ENABLE(Close, Event::EventBase)
        };
    };
}

REFLECTION_TYPE(System::WindowEvents::Move, Event::EventBase)
REFLECTION_TYPE(System::WindowEvents::Resize, Event::EventBase)
REFLECTION_TYPE(System::WindowEvents::Focus, Event::EventBase)
REFLECTION_TYPE(System::WindowEvents::Close, Event::EventBase)
