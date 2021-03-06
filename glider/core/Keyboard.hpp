#ifndef __GLIDER_KEYBOARD_HPP__
#define __GLIDER_KEYBOARD_HPP__

namespace glider{

namespace keyboard{

enum KeySymbol{
  GK_UNKNOWN,
  GK_A,
  GK_B,
  GK_C,
  GK_D,
  GK_E,
  GK_F,
  GK_G,
  GK_H,
  GK_I,
  GK_J,
  GK_K,
  GK_L,
  GK_M,
  GK_N,
  GK_O,
  GK_P,
  GK_Q,
  GK_R,
  GK_S,
  GK_T,
  GK_U,
  GK_V,
  GK_W,
  GK_X,
  GK_Y,
  GK_Z,
  GK_1,
  GK_2,
  GK_3,
  GK_4,
  GK_5,
  GK_6,
  GK_7,
  GK_8,
  GK_9,
  GK_0,
  GK_RETURN,
  GK_ESCAPE,
  GK_BACKSPACE,
  GK_TAB,
  GK_SPACE,
  GK_MINUS,
  GK_EQUALS,
  GK_LEFTBRACKET,
  GK_RIGHTBRACKET,
  GK_BACKSLASH,
  GK_NONUSHASH,
  GK_SEMICOLON,
  GK_APOSTROPHE,
  GK_GRAVE,
  GK_COMMA,
  GK_PERIOD,
  GK_SLASH,
  GK_CAPSLOCK,
  GK_F1,
  GK_F2,
  GK_F3,
  GK_F4,
  GK_F5,
  GK_F6,
  GK_F7,
  GK_F8,
  GK_F9,
  GK_F10,
  GK_F11,
  GK_F12,
  GK_PRINTSCREEN,
  GK_SCROLLLOCK,
  GK_PAUSE,
  GK_INSERT,
  GK_HOME,
  GK_PAGEUP,
  GK_DELETE,
  GK_END,
  GK_PAGEDOWN,
  GK_RIGHT,
  GK_LEFT,
  GK_DOWN,
  GK_UP,
  GK_NUMLOCKCLEAR,
  GK_KP_DIVIDE,
  GK_KP_MULTIPLY,
  GK_KP_MINUS,
  GK_KP_PLUS,
  GK_KP_ENTER,
  GK_KP_1,
  GK_KP_2,
  GK_KP_3,
  GK_KP_4,
  GK_KP_5,
  GK_KP_6,
  GK_KP_7,
  GK_KP_8,
  GK_KP_9,
  GK_KP_0,
  GK_KP_PERIOD,
  GK_NONUSBACKSLASH,
  GK_APPLICATION,
  GK_POWER,
  GK_KP_EQUALS,
  GK_F13,
  GK_F14,
  GK_F15,
  GK_F16,
  GK_F17,
  GK_F18,
  GK_F19,
  GK_F20,
  GK_F21,
  GK_F22,
  GK_F23,
  GK_F24,
  GK_EXECUTE,
  GK_HELP,
  GK_MENU,
  GK_SELECT,
  GK_STOP,
  GK_AGAIN,
  GK_UNDO,
  GK_CUT,
  GK_COPY,
  GK_PASTE,
  GK_FIND,
  GK_MUTE,
  GK_VOLUMEUP,
  GK_VOLUMEDOWN,
  GK_KP_COMMA,
  GK_KP_EQUALSAS400,
  GK_INTERNATIONAL1,
  GK_INTERNATIONAL2,
  GK_INTERNATIONAL3,
  GK_INTERNATIONAL4,
  GK_INTERNATIONAL5,
  GK_INTERNATIONAL6,
  GK_INTERNATIONAL7,
  GK_INTERNATIONAL8,
  GK_INTERNATIONAL9,
  GK_LANG1,
  GK_LANG2,
  GK_LANG3,
  GK_LANG4,
  GK_LANG5,
  GK_LANG6,
  GK_LANG7,
  GK_LANG8,
  GK_LANG9,
  GK_ALTERASE,
  GK_SYSREQ,
  GK_CANCEL,
  GK_CLEAR,
  GK_PRIOR,
  GK_RETURN2,
  GK_SEPARATOR,
  GK_OUT,
  GK_OPER,
  GK_CLEARAGAIN,
  GK_CRSEL,
  GK_EXSEL,
  GK_KP_00,
  GK_KP_000,
  GK_THOUSANDSSEPARATOR,
  GK_DECIMALSEPARATOR,
  GK_CURRENCYUNIT,
  GK_CURRENCYSUBUNIT,
  GK_KP_LEFTPAREN,
  GK_KP_RIGHTPAREN,
  GK_KP_LEFTBRACE,
  GK_KP_RIGHTBRACE,
  GK_KP_TAB,
  GK_KP_BACKSPACE,
  GK_KP_A,
  GK_KP_B,
  GK_KP_C,
  GK_KP_D,
  GK_KP_E,
  GK_KP_F,
  GK_KP_XOR,
  GK_KP_POWER,
  GK_KP_PERCENT,
  GK_KP_LESS,
  GK_KP_GREATER,
  GK_KP_AMPERSAND,
  GK_KP_DBLAMPERSAND,
  GK_KP_VERTICALBAR,
  GK_KP_DBLVERTICALBAR,
  GK_KP_COLON,
  GK_KP_HASH,
  GK_KP_SPACE,
  GK_KP_AT,
  GK_KP_EXCLAM,
  GK_KP_MEMSTORE,
  GK_KP_MEMRECALL,
  GK_KP_MEMCLEAR,
  GK_KP_MEMADD,
  GK_KP_MEMSUBTRACT,
  GK_KP_MEMMULTIPLY,
  GK_KP_MEMDIVIDE,
  GK_KP_PLUSMINUS,
  GK_KP_CLEAR,
  GK_KP_CLEARENTRY,
  GK_KP_BINARY,
  GK_KP_OCTAL,
  GK_KP_DECIMAL,
  GK_KP_HEXADECIMAL,
  GK_LCTRL,
  GK_LSHIFT,
  GK_LALT,
  GK_LGUI,
  GK_RCTRL,
  GK_RSHIFT,
  GK_RALT,
  GK_RGUI,
  GK_MODE,
  GK_AUDIONEXT,
  GK_AUDIOPREV,
  GK_AUDIOSTOP,
  GK_AUDIOPLAY,
  GK_AUDIOMUTE,
  GK_MEDIASELECT,
  GK_WWW,
  GK_MAIL,
  GK_CALCULATOR,
  GK_COMPUTER,
  GK_AC_SEARCH,
  GK_AC_HOME,
  GK_AC_BACK,
  GK_AC_FORWARD,
  GK_AC_STOP,
  GK_AC_REFRESH,
  GK_AC_BOOKMARKS,
  GK_BRIGHTNESSDOWN,
  GK_BRIGHTNESSUP,
  GK_DISPLAYSWITCH,
  GK_KBDILLUMTOGGLE,
  GK_KBDILLUMDOWN,
  GK_KBDILLUMUP,
  GK_EJECT,
  GK_SLEEP,
  GK_APP1,
  GK_APP2
};


enum KeyModifier{
  GK_MOD_NONE     = 0x0000,
  GK_MOD_LSHIFT   = 0x0001,
  GK_MOD_RSHIFT   = 0x0002,
  GK_MOD_SHIFT    = GK_MOD_LSHIFT|GK_MOD_RSHIFT,
  GK_MOD_LCTRL    = 0x0040,
  GK_MOD_RCTRL    = 0x0080,
  GK_MOD_CTRL     = GK_MOD_LCTRL|GK_MOD_RCTRL,
  GK_MOD_LALT     = 0x0100,
  GK_MOD_RALT     = 0x0200,
  GK_MOD_ALT      = GK_MOD_LALT|GK_MOD_RALT,
  GK_MOD_LMETA    = 0x0400,
  GK_MOD_RMETA    = 0x0800,
  GK_MOD_META     = GK_MOD_LMETA|GK_MOD_RMETA,
  GK_MOD_NUM      = 0x1000,
  GK_MOD_CAPS     = 0x2000,
  GK_MOD_MODE     = 0x4000,
  GK_MOD_RESERVED = 0x8000
};

KeySymbol scanCodeToKeySymbol(int scanCode);
}
const char* getModName(keyboard::KeyModifier mod);
const char* getKeyName(keyboard::KeySymbol key);
}

#endif
