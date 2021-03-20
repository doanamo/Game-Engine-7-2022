/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#pragma once

/*
    Window Events
*/

namespace System
{
    struct WindowEvents
    {
        struct Move
        {
            int x;
            int y;
        };

        struct Resize
        {
            int width;
            int height;
        };

        struct Focus
        {
            bool focused;
        };

        struct Close
        {
        };
    };
}
