/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#include "System/Precompiled.hpp"
#include "System/InputDefinitions.hpp"

namespace System
{
    bool IsInputStatePressed(InputStates::Type state, bool repeat)
    {
        return state == InputStates::Pressed || state == InputStates::PressedReleased || (state == InputStates::PressedRepeat && repeat);
    }

    bool IsInputStateReleased(InputStates::Type state, bool repeat)
    {
        return state == InputStates::Released || (state == InputStates::ReleasedRepeat && repeat);
    }

    bool IsInputStateRepeating(InputStates::Type state)
    {
        return state == InputStates::PressedRepeat || state == InputStates::ReleasedRepeat;
    }

    InputStates::Type TransitionInputState(InputStates::Type state)
    {
        switch(state)
        {
            case InputStates::Pressed: return InputStates::PressedRepeat;
            case InputStates::Released: return InputStates::ReleasedRepeat;
            case InputStates::PressedReleased: return InputStates::Released;
        }

        return state;
    }

    InputStates::Type TranslateInputAction(int action)
    {
        switch(action)
        {
            case GLFW_PRESS: return InputStates::Pressed;
            case GLFW_REPEAT: return InputStates::PressedRepeat;
            case GLFW_RELEASE: return InputStates::Released;
        }

        ASSERT(false, "Unexpected input action argument!");
        return InputStates::Invalid;
    }

    KeyboardKeys::Type TranslateKeyboardKey(int key)
    {
        switch(key)
        {
            case GLFW_KEY_UNKNOWN: return KeyboardKeys::KeyUnknown;

            case GLFW_KEY_0: return KeyboardKeys::Key0;
            case GLFW_KEY_1: return KeyboardKeys::Key1;
            case GLFW_KEY_2: return KeyboardKeys::Key2;
            case GLFW_KEY_3: return KeyboardKeys::Key3;
            case GLFW_KEY_4: return KeyboardKeys::Key4;
            case GLFW_KEY_5: return KeyboardKeys::Key5;
            case GLFW_KEY_6: return KeyboardKeys::Key6;
            case GLFW_KEY_7: return KeyboardKeys::Key7;
            case GLFW_KEY_8: return KeyboardKeys::Key8;
            case GLFW_KEY_9: return KeyboardKeys::Key9;

            case GLFW_KEY_A: return KeyboardKeys::KeyA;
            case GLFW_KEY_B: return KeyboardKeys::KeyB;
            case GLFW_KEY_C: return KeyboardKeys::KeyC;
            case GLFW_KEY_D: return KeyboardKeys::KeyD;
            case GLFW_KEY_E: return KeyboardKeys::KeyE;
            case GLFW_KEY_F: return KeyboardKeys::KeyF;
            case GLFW_KEY_G: return KeyboardKeys::KeyG;
            case GLFW_KEY_H: return KeyboardKeys::KeyH;
            case GLFW_KEY_I: return KeyboardKeys::KeyI;
            case GLFW_KEY_J: return KeyboardKeys::KeyJ;
            case GLFW_KEY_K: return KeyboardKeys::KeyK;
            case GLFW_KEY_L: return KeyboardKeys::KeyL;
            case GLFW_KEY_M: return KeyboardKeys::KeyM;
            case GLFW_KEY_N: return KeyboardKeys::KeyN;
            case GLFW_KEY_O: return KeyboardKeys::KeyO;
            case GLFW_KEY_P: return KeyboardKeys::KeyP;
            case GLFW_KEY_Q: return KeyboardKeys::KeyQ;
            case GLFW_KEY_R: return KeyboardKeys::KeyR;
            case GLFW_KEY_S: return KeyboardKeys::KeyS;
            case GLFW_KEY_T: return KeyboardKeys::KeyT;
            case GLFW_KEY_U: return KeyboardKeys::KeyU;
            case GLFW_KEY_V: return KeyboardKeys::KeyV;
            case GLFW_KEY_W: return KeyboardKeys::KeyW;
            case GLFW_KEY_X: return KeyboardKeys::KeyX;
            case GLFW_KEY_Y: return KeyboardKeys::KeyY;
            case GLFW_KEY_Z: return KeyboardKeys::KeyZ;

            case GLFW_KEY_SPACE: return KeyboardKeys::KeySpace;
            case GLFW_KEY_APOSTROPHE: return KeyboardKeys::KeyApostrophe;
            case GLFW_KEY_COMMA: return KeyboardKeys::KeyComma;
            case GLFW_KEY_MINUS: return KeyboardKeys::KeyMinus;
            case GLFW_KEY_PERIOD: return KeyboardKeys::KeyPeriod;
            case GLFW_KEY_SLASH: return KeyboardKeys::KeySlash;
            case GLFW_KEY_SEMICOLON: return KeyboardKeys::KeySemicolon;
            case GLFW_KEY_EQUAL: return KeyboardKeys::KeyEqual;
            case GLFW_KEY_BACKSLASH: return KeyboardKeys::KeyBackslash;
            case GLFW_KEY_LEFT_BRACKET: return KeyboardKeys::KeyLeftBracket;
            case GLFW_KEY_RIGHT_BRACKET: return KeyboardKeys::KeyRightBracket;
            case GLFW_KEY_GRAVE_ACCENT: return KeyboardKeys::KeyTilde;

            case GLFW_KEY_ESCAPE: return KeyboardKeys::KeyEscape;
            case GLFW_KEY_ENTER: return KeyboardKeys::KeyEnter;
            case GLFW_KEY_TAB: return KeyboardKeys::KeyTab;
            case GLFW_KEY_BACKSPACE: return KeyboardKeys::KeyBackspace;
            case GLFW_KEY_INSERT: return KeyboardKeys::KeyInsert;
            case GLFW_KEY_DELETE: return KeyboardKeys::KeyDelete;
            case GLFW_KEY_RIGHT: return KeyboardKeys::KeyRight;
            case GLFW_KEY_LEFT: return KeyboardKeys::KeyLeft;
            case GLFW_KEY_UP: return KeyboardKeys::KeyUp;
            case GLFW_KEY_DOWN: return KeyboardKeys::KeyDown;
            case GLFW_KEY_PAGE_UP: return KeyboardKeys::KeyPageUp;
            case GLFW_KEY_PAGE_DOWN: return KeyboardKeys::KeyPageDown;
            case GLFW_KEY_HOME: return KeyboardKeys::KeyHome;
            case GLFW_KEY_END: return KeyboardKeys::KeyEnd;
            case GLFW_KEY_CAPS_LOCK: return KeyboardKeys::KeyCapsLock;
            case GLFW_KEY_SCROLL_LOCK: return KeyboardKeys::KeyScrollLock;
            case GLFW_KEY_NUM_LOCK: return KeyboardKeys::KeyNumLock;
            case GLFW_KEY_PRINT_SCREEN: return KeyboardKeys::KeyPrintScreen;
            case GLFW_KEY_PAUSE: return KeyboardKeys::KeyPause;

            case GLFW_KEY_F1: return KeyboardKeys::KeyF1;
            case GLFW_KEY_F2: return KeyboardKeys::KeyF2;
            case GLFW_KEY_F3: return KeyboardKeys::KeyF3;
            case GLFW_KEY_F4: return KeyboardKeys::KeyF4;
            case GLFW_KEY_F5: return KeyboardKeys::KeyF5;
            case GLFW_KEY_F6: return KeyboardKeys::KeyF6;
            case GLFW_KEY_F7: return KeyboardKeys::KeyF7;
            case GLFW_KEY_F8: return KeyboardKeys::KeyF8;
            case GLFW_KEY_F9: return KeyboardKeys::KeyF9;
            case GLFW_KEY_F10: return KeyboardKeys::KeyF10;
            case GLFW_KEY_F11: return KeyboardKeys::KeyF11;
            case GLFW_KEY_F12: return KeyboardKeys::KeyF12;

            case GLFW_KEY_KP_0: return KeyboardKeys::KeyNum0;
            case GLFW_KEY_KP_1: return KeyboardKeys::KeyNum1;
            case GLFW_KEY_KP_2: return KeyboardKeys::KeyNum2;
            case GLFW_KEY_KP_3: return KeyboardKeys::KeyNum3;
            case GLFW_KEY_KP_4: return KeyboardKeys::KeyNum4;
            case GLFW_KEY_KP_5: return KeyboardKeys::KeyNum5;
            case GLFW_KEY_KP_6: return KeyboardKeys::KeyNum6;
            case GLFW_KEY_KP_7: return KeyboardKeys::KeyNum7;
            case GLFW_KEY_KP_8: return KeyboardKeys::KeyNum8;
            case GLFW_KEY_KP_9: return KeyboardKeys::KeyNum9;

            case GLFW_KEY_KP_DECIMAL: return KeyboardKeys::KeyNumDecimal;
            case GLFW_KEY_KP_DIVIDE: return KeyboardKeys::KeyNumDivide;
            case GLFW_KEY_KP_MULTIPLY: return KeyboardKeys::KeyNumMultiply;
            case GLFW_KEY_KP_SUBTRACT: return KeyboardKeys::KeyNumSubtract;
            case GLFW_KEY_KP_ADD: return KeyboardKeys::KeyNumAdd;
            case GLFW_KEY_KP_ENTER: return KeyboardKeys::KeyNumEnter;
            case GLFW_KEY_KP_EQUAL: return KeyboardKeys::KeyNumEqual;
            
            case GLFW_KEY_LEFT_SHIFT: return KeyboardKeys::KeyLeftShift;
            case GLFW_KEY_RIGHT_SHIFT: return KeyboardKeys::KeyRightShift;
            case GLFW_KEY_LEFT_CONTROL: return KeyboardKeys::KeyLeftCtrl;
            case GLFW_KEY_RIGHT_CONTROL: return KeyboardKeys::KeyRightCtrl;
            case GLFW_KEY_LEFT_ALT: return KeyboardKeys::KeyLeftAlt;
            case GLFW_KEY_RIGHT_ALT: return KeyboardKeys::KeyRightAlt;
            case GLFW_KEY_LEFT_SUPER: return KeyboardKeys::KeyLeftSuper;
            case GLFW_KEY_RIGHT_SUPER: return KeyboardKeys::KeyRightSuper;
            case GLFW_KEY_MENU: return KeyboardKeys::KeyMenu;
        }

        ASSERT(false, "Unexpected keyboard key argument!");
        return KeyboardKeys::Invalid;
    }

    KeyboardModifiers::Type TranslateKeyboardModifiers(int mods)
    {
        KeyboardModifiers::Type modifiers = KeyboardModifiers::None;
        if(mods & GLFW_MOD_ALT) modifiers |= KeyboardModifiers::Alt;
        if(mods & GLFW_MOD_SHIFT) modifiers |= KeyboardModifiers::Shift;
        if(mods & GLFW_MOD_CONTROL) modifiers |= KeyboardModifiers::Ctrl;
        if(mods & GLFW_MOD_SUPER) modifiers |= KeyboardModifiers::Super;
        return modifiers;
    }

    MouseButtons::Type TranslateMouseButton(int button)
    {
        MouseButtons::Type result = MouseButtons::Button1 + button;
        ASSERT(result >= MouseButtons::Button1 && button < MouseButtons::Count, "Unexpected mouse button index argument!");
        return result < MouseButtons::Count ? result : MouseButtons::Invalid;
    }
}
