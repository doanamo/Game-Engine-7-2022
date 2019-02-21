/*
    Copyright (c) 2018-2019 Piotr Doan. All rights reserved.
*/

#pragma once

/*
    Input Defines
*/

namespace System
{
    // General input actions.
    struct InputStates
    {
        using Type = unsigned short;

        enum
        {
            Invalid,

            Pressed,
            PressedRepeat,
            Released,
            ReleasedRepeat,

            Count,
        };
    };

    InputStates::Type TranslateInputAction(int action);

    // Keyboard input keys.
    struct KeyboardKeys
    {
        using Type = unsigned short;

        enum
        {
            Invalid,

            KeyUnknown,

            Key0,
            Key1,
            Key2,
            Key3,
            Key4,
            Key5,
            Key6,
            Key7,
            Key8,
            Key9,

            KeyA,
            KeyB,
            KeyC,
            KeyD,
            KeyE,
            KeyF,
            KeyG,
            KeyH,
            KeyI,
            KeyJ,
            KeyK,
            KeyL,
            KeyM,
            KeyN,
            KeyO,
            KeyP,
            KeyQ,
            KeyR,
            KeyS,
            KeyT,
            KeyU,
            KeyV,
            KeyW,
            KeyX,
            KeyY,
            KeyZ,

            KeySpace,
            KeyApostrophe,
            KeyComma,
            KeyMinus,
            KeyPeriod,
            KeySlash,
            KeySemicolon,
            KeyEqual,
            KeyBackslash,
            KeyLeftBracket,
            KeyRightBracket,
            KeyTilde,

            KeyEscape,
            KeyEnter,
            KeyTab,
            KeyBackspace,
            KeyInsert,
            KeyDelete,
            KeyRight,
            KeyLeft,
            KeyUp,
            KeyDown,
            KeyPageUp,
            KeyPageDown,
            KeyHome,
            KeyEnd,
            KeyCapsLock,
            KeyScrollLock,
            KeyNumLock,
            KeyPrintScreen,
            KeyPause,

            KeyF1,
            KeyF2,
            KeyF3,
            KeyF4,
            KeyF5,
            KeyF6,
            KeyF7,
            KeyF8,
            KeyF9,
            KeyF10,
            KeyF11,
            KeyF12,

            KeyNum0,
            KeyNum1,
            KeyNum2,
            KeyNum3,
            KeyNum4,
            KeyNum5,
            KeyNum6,
            KeyNum7,
            KeyNum8,
            KeyNum9,

            KeyNumDecimal,
            KeyNumDivide,
            KeyNumMultiply,
            KeyNumSubtract,
            KeyNumAdd,
            KeyNumEnter,
            KeyNumEqual,

            KeyLeftShift,
            KeyRightShift,
            KeyLeftCtrl,
            KeyRightCtrl,
            KeyLeftAlt,
            KeyRightAlt,
            KeyLeftSuper,
            KeyRightSuper,
            KeyMenu,

            Count,
        };
    };

    KeyboardKeys::Type TranslateKeyboardKey(int key);

    // Keyboard input modifiers.
    struct KeyboardModifiers
    {
        using Type = unsigned short;

        enum
        {
            Invalid,

            None = 0 << 0,
            Shift = 1 << 0,
            Ctrl = 1 << 1,
            Alt = 1 << 2,
            Super = 1 << 3,

            Count,
        };
    };

    KeyboardModifiers::Type TranslateKeyboardModifiers(int mods);

    // Mouse input buttons.
    struct MouseButtons
    {
        using Type = unsigned short;

        enum
        {
            Invalid,

            Button1,
            Button2,
            Button3,
            Button4,
            Button5,
            Button6,
            Button7,
            Button8,

            ButtonLeft = Button1,
            ButtonRight = Button2,
            ButtonMiddle = Button3,

            Count,
        };
    };

    MouseButtons::Type TranslateMouseButton(int button);

    // Input event structures.
    namespace InputEvents
    {
        // Keyboard key event.
        struct KeyboardKey
        {
            KeyboardKey() :
                state(InputStates::Invalid),
                key(KeyboardKeys::Invalid),
                modifiers(KeyboardModifiers::Invalid),
                scancode(0)
            {
            }

            InputStates::Type state;
            KeyboardKeys::Type key;
            KeyboardModifiers::Type modifiers;
            int scancode;
        };

        // Text input event.
        struct TextInput
        {
            TextInput() :
                utf32Character(0)
            {
            }

            // Character is stored in UTF32 format here and can be
            // converted to any other code point as the user wishes.
            unsigned int utf32Character;
        };

        // Mouse button event.
        struct MouseButton
        {
            MouseButton() :
                state(InputStates::Invalid),
                button(MouseButtons::Invalid),
                modifiers(KeyboardModifiers::Invalid)
            {
            }

            InputStates::Type state;
            MouseButtons::Type button;
            KeyboardKeys::Type modifiers;
        };

        // Mouse scroll event.
        struct MouseScroll
        {
            MouseScroll() :
                offset(0.0)
            {
            }

            double offset;
        };

        // Cursor position event.
        struct CursorPosition
        {
            CursorPosition() :
                x(0.0),
                y(0.0)
            {
            }

            double x;
            double y;
        };

        // Cursor enter events.
        struct CursorEnter
        {
            CursorEnter() :
                entered(false)
            {
            }

            bool entered;
        };
    }
}
