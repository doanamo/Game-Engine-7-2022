/*
    Copyright (c) 2018-2020 Piotr Doan. All rights reserved.
*/

#pragma once

/*
    Input Defines
*/

namespace System
{
    struct InputStates
    {
        using Type = unsigned short;

        enum
        {
            Invalid,

            Pressed,         // Input was pressed just before processed update.
            PressedRepeat,   // Input was pressed for more than one update.
            PressedReleased, // Input was pressed and released in one update.
            Released,        // Input was released just before processed update.
            ReleasedRepeat,  // Input was released for more than one update.

            Count,
        };
    };

    bool IsInputStatePressed(InputStates::Type state, bool repeat = true);
    bool IsInputStateReleased(InputStates::Type state, bool repeat = true);
    bool IsInputStateRepeating(InputStates::Type state);

    InputStates::Type TransitionInputState(InputStates::Type state);
    InputStates::Type TranslateInputAction(int action);

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

            Count,

            ButtonLeft = Button1,
            ButtonRight = Button2,
            ButtonMiddle = Button3,
        };
    };

    MouseButtons::Type TranslateMouseButton(int button);

    namespace InputEvents
    {
        struct TextInput
        {
            unsigned int utf32Character = 0;
        };

        struct KeyboardKey
        {
            KeyboardKeys::Type key = KeyboardKeys::Invalid;
            KeyboardModifiers::Type modifiers = KeyboardModifiers::None;
            InputStates::Type state = InputStates::ReleasedRepeat;
            float stateTime = 0.0f;
        };

        struct MouseButton
        {
            MouseButtons::Type button = MouseButtons::Invalid;
            KeyboardKeys::Type modifiers = KeyboardModifiers::Invalid;
            InputStates::Type state = InputStates::ReleasedRepeat;
            float stateTime = 0.0f;
        };

        struct MouseScroll
        {
            double offset = 0.0;
        };

        struct CursorPosition
        {
            double x = 0.0;
            double y = 0.0;
        };

        struct CursorEnter
        {
            bool entered = false;
        };
    }
}
