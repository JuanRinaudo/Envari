SDL_SCANCODE_UNKNOWN = 1;

--[[
*  \name Usage page 0x07
*
*  These values are from usage page 0x07 (USB keyboard page).
--]]

SDL_SCANCODE_A = 5;
SDL_SCANCODE_B = 6;
SDL_SCANCODE_C = 7;
SDL_SCANCODE_D = 8;
SDL_SCANCODE_E = 9;
SDL_SCANCODE_F = 10;
SDL_SCANCODE_G = 11;
SDL_SCANCODE_H = 12;
SDL_SCANCODE_I = 13;
SDL_SCANCODE_J = 14;
SDL_SCANCODE_K = 15;
SDL_SCANCODE_L = 16;
SDL_SCANCODE_M = 17;
SDL_SCANCODE_N = 18;
SDL_SCANCODE_O = 19;
SDL_SCANCODE_P = 20;
SDL_SCANCODE_Q = 21;
SDL_SCANCODE_R = 22;
SDL_SCANCODE_S = 23;
SDL_SCANCODE_T = 24;
SDL_SCANCODE_U = 25;
SDL_SCANCODE_V = 26;
SDL_SCANCODE_W = 27;
SDL_SCANCODE_X = 28;
SDL_SCANCODE_Y = 29;
SDL_SCANCODE_Z = 30;

SDL_SCANCODE_1 = 31;
SDL_SCANCODE_2 = 32;
SDL_SCANCODE_3 = 33;
SDL_SCANCODE_4 = 34;
SDL_SCANCODE_5 = 35;
SDL_SCANCODE_6 = 36;
SDL_SCANCODE_7 = 37;
SDL_SCANCODE_8 = 38;
SDL_SCANCODE_9 = 39;
SDL_SCANCODE_0 = 40;

SDL_SCANCODE_RETURN = 41;
SDL_SCANCODE_ESCAPE = 42;
SDL_SCANCODE_BACKSPACE = 43;
SDL_SCANCODE_TAB = 44;
SDL_SCANCODE_SPACE = 45;

SDL_SCANCODE_MINUS = 46;
SDL_SCANCODE_EQUALS = 47;
SDL_SCANCODE_LEFTBRACKET = 48;
SDL_SCANCODE_RIGHTBRACKET = 49;
SDL_SCANCODE_BACKSLASH = 50; --[[ Located at the lower left of the return
                              *   key on ISO keyboards and at the right end
                              *   of the QWERTY row on ANSI keyboards.
                              *   Produces REVERSE SOLIDUS (backslash) and
                              *   VERTICAL LINE in a US layout; REVERSE
                              *   SOLIDUS and VERTICAL LINE in a UK Mac
                              *   layout; NUMBER SIGN and TILDE in a UK
                              *   Windows layout; DOLLAR SIGN and POUND SIGN
                              *   in a Swiss German layout; NUMBER SIGN and
                              *   APOSTROPHE in a German layout; GRAVE
                              *   ACCENT and POUND SIGN in a French Mac
                              *   layout; and ASTERISK and MICRO SIGN in a
                              *   French Windows layout.
                              --]]
SDL_SCANCODE_NONUSHASH = 51; --[[ ISO USB keyboards actually use this code
                              *   instead of 49 for the same key; but all
                              *   OSes I've seen treat the two codes
                              *   identically. So; as an implementor; unless
                              *   your keyboard generates both of those
                              *   codes and your OS treats them differently;
                              *   you should generate SDL_SCANCODE_BACKSLASH
                              *   instead of this code. As a user; you
                              *   should not rely on this code because SDL
                              *   will never generate it with most (all?)
                              *   keyboards.
                              --]]
SDL_SCANCODE_SEMICOLON = 52;
SDL_SCANCODE_APOSTROPHE = 53;
SDL_SCANCODE_GRAVE = 54; --[[ Located in the top left corner (on both ANSI
                        *   and ISO keyboards). Produces GRAVE ACCENT and
                        *   TILDE in a US Windows layout and in US and UK
                        *   Mac layouts on ANSI keyboards; GRAVE ACCENT
                        *   and NOT SIGN in a UK Windows layout; SECTION
                        *   SIGN and PLUS-MINUS SIGN in US and UK Mac
                        *   layouts on ISO keyboards; SECTION SIGN and
                        *   DEGREE SIGN in a Swiss German layout (Mac:
                        *   only on ISO keyboards); CIRCUMFLEX ACCENT and
                        *   DEGREE SIGN in a German layout (Mac: only on
                        *   ISO keyboards); SUPERSCRIPT TWO and TILDE in a
                        *   French Windows layout; COMMERCIAL AT and
                        *   NUMBER SIGN in a French Mac layout on ISO
                        *   keyboards; and LESS-THAN SIGN and GREATER-THAN
                        *   SIGN in a Swiss German; German; or French Mac
                        *   layout on ANSI keyboards.
                        --]]
SDL_SCANCODE_COMMA = 55;
SDL_SCANCODE_PERIOD = 56;
SDL_SCANCODE_SLASH = 57;

SDL_SCANCODE_CAPSLOCK = 58;

SDL_SCANCODE_F1 = 59;
SDL_SCANCODE_F2 = 60;
SDL_SCANCODE_F3 = 61;
SDL_SCANCODE_F4 = 62;
SDL_SCANCODE_F5 = 63;
SDL_SCANCODE_F6 = 64;
SDL_SCANCODE_F7 = 65;
SDL_SCANCODE_F8 = 66;
SDL_SCANCODE_F9 = 67;
SDL_SCANCODE_F10 = 68;
SDL_SCANCODE_F11 = 69;
SDL_SCANCODE_F12 = 70;

SDL_SCANCODE_PRINTSCREEN = 71;
SDL_SCANCODE_SCROLLLOCK = 72;
SDL_SCANCODE_PAUSE = 73;
SDL_SCANCODE_INSERT = 74; --[[ insert on PC; help on some Mac keyboards (but
                              does send code 73; not 117) --]]
SDL_SCANCODE_HOME = 75;
SDL_SCANCODE_PAGEUP = 76;
SDL_SCANCODE_DELETE = 77;
SDL_SCANCODE_END = 78;
SDL_SCANCODE_PAGEDOWN = 79;
SDL_SCANCODE_RIGHT = 80;
SDL_SCANCODE_LEFT = 81;
SDL_SCANCODE_DOWN = 82;
SDL_SCANCODE_UP = 83;

SDL_SCANCODE_NUMLOCKCLEAR = 84; --[[ num lock on PC; clear on Mac keyboards
                                 --]]
SDL_SCANCODE_KP_DIVIDE = 85;
SDL_SCANCODE_KP_MULTIPLY = 86;
SDL_SCANCODE_KP_MINUS = 87;
SDL_SCANCODE_KP_PLUS = 88;
SDL_SCANCODE_KP_ENTER = 89;
SDL_SCANCODE_KP_1 = 90;
SDL_SCANCODE_KP_2 = 91;
SDL_SCANCODE_KP_3 = 92;
SDL_SCANCODE_KP_4 = 93;
SDL_SCANCODE_KP_5 = 94;
SDL_SCANCODE_KP_6 = 95;
SDL_SCANCODE_KP_7 = 96;
SDL_SCANCODE_KP_8 = 97;
SDL_SCANCODE_KP_9 = 98;
SDL_SCANCODE_KP_0 = 99;
SDL_SCANCODE_KP_PERIOD = 100;

SDL_SCANCODE_NONUSBACKSLASH = 101; --[[ This is the additional key that ISO
                                    *   keyboards have over ANSI ones;
                                    *   located between left shift and Y.
                                    *   Produces GRAVE ACCENT and TILDE in a
                                    *   US or UK Mac layout; REVERSE SOLIDUS
                                    *   (backslash) and VERTICAL LINE in a
                                    *   US or UK Windows layout; and
                                    *   LESS-THAN SIGN and GREATER-THAN SIGN
                                    *   in a Swiss German; German; or French
                                    *   layout. --]]
SDL_SCANCODE_APPLICATION = 102; --[[ windows contextual menu; compose --]]
SDL_SCANCODE_POWER = 103; --[[ The USB document says this is a status flag;
                           *   not a physical key - but some Mac keyboards
                           *   do have a power key. --]]
SDL_SCANCODE_KP_EQUALS = 104;
SDL_SCANCODE_F13 = 105;
SDL_SCANCODE_F14 = 106;
SDL_SCANCODE_F15 = 107;
SDL_SCANCODE_F16 = 108;
SDL_SCANCODE_F17 = 109;
SDL_SCANCODE_F18 = 110;
SDL_SCANCODE_F19 = 111;
SDL_SCANCODE_F20 = 112;
SDL_SCANCODE_F21 = 113;
SDL_SCANCODE_F22 = 114;
SDL_SCANCODE_F23 = 115;
SDL_SCANCODE_F24 = 116;
SDL_SCANCODE_EXECUTE = 117;
SDL_SCANCODE_HELP = 118;
SDL_SCANCODE_MENU = 119;
SDL_SCANCODE_SELECT = 120;
SDL_SCANCODE_STOP = 121;
SDL_SCANCODE_AGAIN = 122;   --[[ redo --]]
SDL_SCANCODE_UNDO = 123;
SDL_SCANCODE_CUT = 124;
SDL_SCANCODE_COPY = 125;
SDL_SCANCODE_PASTE = 126;
SDL_SCANCODE_FIND = 127;
SDL_SCANCODE_MUTE = 128;
SDL_SCANCODE_VOLUMEUP = 129;
SDL_SCANCODE_VOLUMEDOWN = 130;
--]] not sure whether there's a reason to enable these --]]
--]]     SDL_SCANCODE_LOCKINGCAPSLOCK = 130;  --]]
--]]     SDL_SCANCODE_LOCKINGNUMLOCK = 131; --]]
--]]     SDL_SCANCODE_LOCKINGSCROLLLOCK = 132; --]]
SDL_SCANCODE_KP_COMMA = 134;
SDL_SCANCODE_KP_EQUALSAS400 = 135;

SDL_SCANCODE_INTERNATIONAL1 = 136; --[[ used on Asian keyboards; see
                                       footnotes in USB doc --]]
SDL_SCANCODE_INTERNATIONAL2 = 137;
SDL_SCANCODE_INTERNATIONAL3 = 138; --[[ Yen --]]
SDL_SCANCODE_INTERNATIONAL4 = 139;
SDL_SCANCODE_INTERNATIONAL5 = 140;
SDL_SCANCODE_INTERNATIONAL6 = 141;
SDL_SCANCODE_INTERNATIONAL7 = 142;
SDL_SCANCODE_INTERNATIONAL8 = 143;
SDL_SCANCODE_INTERNATIONAL9 = 144;
SDL_SCANCODE_LANG1 = 145; --[[ Hangul/English toggle --]]
SDL_SCANCODE_LANG2 = 146; --[[ Hanja conversion --]]
SDL_SCANCODE_LANG3 = 147; --[[ Katakana --]]
SDL_SCANCODE_LANG4 = 148; --[[ Hiragana --]]
SDL_SCANCODE_LANG5 = 149; --[[ Zenkaku/Hankaku --]]
SDL_SCANCODE_LANG6 = 150; --[[ reserved --]]
SDL_SCANCODE_LANG7 = 151; --[[ reserved --]]
SDL_SCANCODE_LANG8 = 152; --[[ reserved --]]
SDL_SCANCODE_LANG9 = 153; --[[ reserved --]]

SDL_SCANCODE_ALTERASE = 154; --[[ Erase-Eaze --]]
SDL_SCANCODE_SYSREQ = 155;
SDL_SCANCODE_CANCEL = 156;
SDL_SCANCODE_CLEAR = 157;
SDL_SCANCODE_PRIOR = 158;
SDL_SCANCODE_RETURN2 = 159;
SDL_SCANCODE_SEPARATOR = 160;
SDL_SCANCODE_OUT = 161;
SDL_SCANCODE_OPER = 162;
SDL_SCANCODE_CLEARAGAIN = 163;
SDL_SCANCODE_CRSEL = 164;
SDL_SCANCODE_EXSEL = 165;

SDL_SCANCODE_KP_00 = 177;
SDL_SCANCODE_KP_000 = 178;
SDL_SCANCODE_THOUSANDSSEPARATOR = 179;
SDL_SCANCODE_DECIMALSEPARATOR = 180;
SDL_SCANCODE_CURRENCYUNIT = 181;
SDL_SCANCODE_CURRENCYSUBUNIT = 182;
SDL_SCANCODE_KP_LEFTPAREN = 183;
SDL_SCANCODE_KP_RIGHTPAREN = 184;
SDL_SCANCODE_KP_LEFTBRACE = 185;
SDL_SCANCODE_KP_RIGHTBRACE = 186;
SDL_SCANCODE_KP_TAB = 187;
SDL_SCANCODE_KP_BACKSPACE = 188;
SDL_SCANCODE_KP_A = 189;
SDL_SCANCODE_KP_B = 190;
SDL_SCANCODE_KP_C = 191;
SDL_SCANCODE_KP_D = 192;
SDL_SCANCODE_KP_E = 193;
SDL_SCANCODE_KP_F = 194;
SDL_SCANCODE_KP_XOR = 195;
SDL_SCANCODE_KP_POWER = 196;
SDL_SCANCODE_KP_PERCENT = 197;
SDL_SCANCODE_KP_LESS = 198;
SDL_SCANCODE_KP_GREATER = 199;
SDL_SCANCODE_KP_AMPERSAND = 200;
SDL_SCANCODE_KP_DBLAMPERSAND = 201;
SDL_SCANCODE_KP_VERTICALBAR = 202;
SDL_SCANCODE_KP_DBLVERTICALBAR = 203;
SDL_SCANCODE_KP_COLON = 204;
SDL_SCANCODE_KP_HASH = 205;
SDL_SCANCODE_KP_SPACE = 206;
SDL_SCANCODE_KP_AT = 207;
SDL_SCANCODE_KP_EXCLAM = 208;
SDL_SCANCODE_KP_MEMSTORE = 209;
SDL_SCANCODE_KP_MEMRECALL = 210;
SDL_SCANCODE_KP_MEMCLEAR = 211;
SDL_SCANCODE_KP_MEMADD = 212;
SDL_SCANCODE_KP_MEMSUBTRACT = 213;
SDL_SCANCODE_KP_MEMMULTIPLY = 214;
SDL_SCANCODE_KP_MEMDIVIDE = 215;
SDL_SCANCODE_KP_PLUSMINUS = 216;
SDL_SCANCODE_KP_CLEAR = 217;
SDL_SCANCODE_KP_CLEARENTRY = 218;
SDL_SCANCODE_KP_BINARY = 219;
SDL_SCANCODE_KP_OCTAL = 220;
SDL_SCANCODE_KP_DECIMAL = 221;
SDL_SCANCODE_KP_HEXADECIMAL = 222;

SDL_SCANCODE_LCTRL = 225;
SDL_SCANCODE_LSHIFT = 226;
SDL_SCANCODE_LALT = 227; --[[ alt; option --]]
SDL_SCANCODE_LGUI = 228; --[[ windows; command (apple); meta --]]
SDL_SCANCODE_RCTRL = 229;
SDL_SCANCODE_RSHIFT = 230;
SDL_SCANCODE_RALT = 231; --[[ alt gr; option --]]
SDL_SCANCODE_RGUI = 232; --[[ windows; command (apple); meta --]]

SDL_SCANCODE_MODE = 258;    --[[ I'm not sure if this is really not covered
                           *   by any of the above; but since there's a
                           *   special KMOD_MODE for it I'm adding it here
                           --]]

--]] Usage page 0x07 --]]

--[[
*  \name Usage page 0x0C
*
*  These values are mapped from usage page 0x0C (USB consumer page).
--]]

SDL_SCANCODE_AUDIONEXT = 259;
SDL_SCANCODE_AUDIOPREV = 260;
SDL_SCANCODE_AUDIOSTOP = 261;
SDL_SCANCODE_AUDIOPLAY = 262;
SDL_SCANCODE_AUDIOMUTE = 263;
SDL_SCANCODE_MEDIASELECT = 264;
SDL_SCANCODE_WWW = 265;
SDL_SCANCODE_MAIL = 266;
SDL_SCANCODE_CALCULATOR = 267;
SDL_SCANCODE_COMPUTER = 268;
SDL_SCANCODE_AC_SEARCH = 269;
SDL_SCANCODE_AC_HOME = 270;
SDL_SCANCODE_AC_BACK = 271;
SDL_SCANCODE_AC_FORWARD = 272;
SDL_SCANCODE_AC_STOP = 273;
SDL_SCANCODE_AC_REFRESH = 274;
SDL_SCANCODE_AC_BOOKMARKS = 275;

--]] Usage page 0x0C --]]

--[[
*  \name Walther keys
*
*  These are values that Christian Walther added (for mac keyboard?).
--]]

SDL_SCANCODE_BRIGHTNESSDOWN = 276;
SDL_SCANCODE_BRIGHTNESSUP = 277;
SDL_SCANCODE_DISPLAYSWITCH = 278; --[[ display mirroring/dual display
                                       switch; video mode switch --]]
SDL_SCANCODE_KBDILLUMTOGGLE = 279;
SDL_SCANCODE_KBDILLUMDOWN = 280;
SDL_SCANCODE_KBDILLUMUP = 281;
SDL_SCANCODE_EJECT = 282;
SDL_SCANCODE_SLEEP = 283;

SDL_SCANCODE_APP1 = 284;
SDL_SCANCODE_APP2 = 285;

--]] Walther keys --]]

--[[
*  \name Usage page 0x0C (additional media keys)
*
*  These values are mapped from usage page 0x0C (USB consumer page).
--]]

SDL_SCANCODE_AUDIOREWIND = 286;
SDL_SCANCODE_AUDIOFASTFORWARD = 287;

--]] Usage page 0x0C (additional media keys) --]]

--]] Add any other keys here. --]]

SDL_NUM_SCANCODES = 513 --[[ not a key; just marks the number of scancodes
                           for array bounds --]]