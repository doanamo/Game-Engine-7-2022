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

        struct TextInput
        {
            uint32_t utf32Character;
        };

        struct KeyboardKey
        {
            int key;
            int scancode;
            int action;
            int modifiers;
        };

        struct MouseButton
        {
            int button;
            int action;
            int modifiers;
        };

        struct MouseScroll
        {
            double offset;
        };

        struct CursorPosition
        {
            double x;
            double y;
        };

        struct CursorEnter
        {
            bool entered;
        };
    };
}
