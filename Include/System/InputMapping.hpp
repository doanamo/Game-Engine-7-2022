/*
    Copyright (c) 2018 Piotr Doan. All rights reserved.
*/

#pragma once

/*
    Input Mapping
*/

namespace System
{
    // General input actions.
    struct InputStates
    {
        using Type = char;

        enum
        {
            Invalid,

            Released,
            Pressed,
            Held,

            Count,
        };
    };

    InputStates::Type TranslateInputState(int action);

    // Keyboard input keys.
    struct KeyboardKeys
    {
        using Type = short;

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
        using Type = unsigned int;

        enum
        {
            Invalid,

            ModShift = 1 << 0,
            ModCtrl = 1 << 1,
            ModAlt = 1 << 2,
            ModSuper = 1 << 3,

            Count,
        };
    };

    KeyboardModifiers::Type TranslateKeyboardModifiers(int mods);

    // Mouse input buttons.
    struct MouseButtons
    {
        using Type = unsigned char;

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
}
