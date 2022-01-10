/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#pragma once

/*
    Event Base

    Base class for events types.
*/

namespace Event
{
    struct EventBase
    {
        REFLECTION_ENABLE(EventBase)
    };
}

REFLECTION_TYPE(Event::EventBase)
