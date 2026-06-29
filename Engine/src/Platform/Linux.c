#include "Platform.h"

#if FPLATFORM_LINUX

#include "Core/Logger.h"
#include "Core/Event.h"
#include "Core/Input.h"

#include <xcb/xcb.h>
#include <xkbcommon/xkbcommon.h>
#include <xkbcommon/xkbcommon-x11.h> // For native XCB key mapping

#include <sys/time.h>

#define XCB_BUTTON_INDEX_6 ((enum xcb_button_index_t)6)
#define XCB_BUTTON_INDEX_7 ((enum xcb_button_index_t)7)
#define XCB_BUTTON_INDEX_8 ((enum xcb_button_index_t)8)
#define XCB_BUTTON_INDEX_9 ((enum xcb_button_index_t)9)

#if _POSIX_C_SOURCE >= 199309L
#include <time.h>  // nanosleep
#else
#include <unistd.h>  // usleep
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct InternalState {
    xcb_connection_t* Connection;
    xcb_window_t Window;
    xcb_screen_t* Screen;
    xcb_atom_t WmProtocols;
    xcb_atom_t WmDeleteWin;

    struct xkb_context* XkbContext;
    struct xkb_keymap* XkbKeymap;
    struct xkb_state* XkbState;
    Int32 XkbDeviceId;
} InternalState;

Keys TranslateKeyCode(xkb_keysym_t XKeycode);

Bool8 PlatformStartup( PlatformState* Platform, const char* WindowName, Int32 X, Int32 Y, Int32 Width, Int32 Height)
{
    Platform->InternalState = malloc(sizeof(InternalState));
    InternalState* State = (InternalState*)Platform->InternalState;

    // 1. Connect directly to X Server via XCB
    int ScreenIdx = 0;
    State->Connection = xcb_connect(NULL, &ScreenIdx);
    if (xcb_connection_has_error(State->Connection)) {
        FLFATAL("Failed to connect to X11 server via raw XCB.");
        return FALSE;
    }

    // 2. Setup modern XKB Keyboard Context (No Xlib dependency)
    State->XkbContext = xkb_context_new(XKB_CONTEXT_NO_FLAGS);
    xkb_x11_setup_xkb_extension(State->Connection, 
                                XKB_X11_MIN_MAJOR_XKB_VERSION, 
                                XKB_X11_MIN_MINOR_XKB_VERSION, 
                                XKB_X11_SETUP_XKB_EXTENSION_NO_FLAGS, 
                                NULL, NULL, NULL, NULL);
    
    State->XkbDeviceId = xkb_x11_get_core_keyboard_device_id(State->Connection);
    State->XkbKeymap = xkb_x11_keymap_new_from_device(State->XkbContext, State->Connection, State->XkbDeviceId, XKB_KEYMAP_COMPILE_NO_FLAGS);
    State->XkbState = xkb_x11_state_new_from_device(State->XkbKeymap, State->Connection, State->XkbDeviceId);

    // 3. Extract your Screen structures using the iterator
    const struct xcb_setup_t* Setup = xcb_get_setup(State->Connection);
    xcb_screen_iterator_t It = xcb_setup_roots_iterator(Setup);
    for (Int32 s = ScreenIdx; s > 0; s--) {
        xcb_screen_next(&It);
    }
    State->Screen = It.data;

    // Allocate a XID for the window to be created.
    State->Window = xcb_generate_id(State->Connection);

    // Register event types.
    // XCB_CW_BACK_PIXEL = filling then window bg with a single colour
    // XCB_CW_EVENT_MASK is required.
    UInt32 EventMask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;

    // Listen for keyboard and mouse buttons
    UInt32 EventValues = XCB_EVENT_MASK_BUTTON_PRESS | XCB_EVENT_MASK_BUTTON_RELEASE |
                       XCB_EVENT_MASK_KEY_PRESS | XCB_EVENT_MASK_KEY_RELEASE |
                       XCB_EVENT_MASK_EXPOSURE | XCB_EVENT_MASK_POINTER_MOTION |
                       XCB_EVENT_MASK_STRUCTURE_NOTIFY;

    // Values to be sent over XCB (bg colour, events)
    UInt32 ValueList[] = {State->Screen->black_pixel, EventValues};

    // Create the window
    xcb_void_cookie_t Cookie = xcb_create_window(
        State->Connection,
        XCB_COPY_FROM_PARENT,  // depth
        State->Window,
        State->Screen->root,            // parent
        X,                              //x
        Y,                              //y
        Width,                          //width
        Height,                         //height
        0,                              // No border
        XCB_WINDOW_CLASS_INPUT_OUTPUT,  //class
        State->Screen->root_visual,
        EventMask,
        ValueList);

        (void)Cookie;

    // Change the title
    xcb_change_property(
        State->Connection,
        XCB_PROP_MODE_REPLACE,
        State->Window,
        XCB_ATOM_WM_NAME,
        XCB_ATOM_STRING,
        8,  // data should be viewed 8 bits at a time
        strlen(WindowName),
        WindowName);

xcb_intern_atom_cookie_t WmDeleteCookie = xcb_intern_atom(
    State->Connection,
    0,
    strlen("WM_DELETE_WINDOW"),
    "WM_DELETE_WINDOW"
);

xcb_intern_atom_cookie_t WmProtocolsCookie = xcb_intern_atom(
    State->Connection,
    0,
    strlen("WM_PROTOCOLS"),
    "WM_PROTOCOLS"
);

xcb_intern_atom_reply_t* WmDeleteReply = xcb_intern_atom_reply(
    State->Connection,
    WmDeleteCookie,
    NULL
);

xcb_intern_atom_reply_t* WmProtocolsReply = xcb_intern_atom_reply(
    State->Connection,
    WmProtocolsCookie,
    NULL
);

State->WmDeleteWin = WmDeleteReply->atom;
State->WmProtocols = WmProtocolsReply->atom;

xcb_change_property(
        State->Connection,
        XCB_PROP_MODE_REPLACE,
        State->Window,
        WmProtocolsReply->atom,
        4,
        32,
        1,
        &WmDeleteReply->atom);

    // Map the window to the screen
    xcb_map_window(State->Connection, State->Window);

    // Flush the stream
    Int32 StreamResult = xcb_flush(State->Connection);
    if (StreamResult <= 0) {
        FLFATAL("An error occurred when flusing the stream: %d", StreamResult);
        return FALSE;
    }

    return TRUE;

}

void PlatformShutdown(PlatformState* Platform)
{
    // Simply cold-cast to the known type.
    InternalState* State = (InternalState*)Platform->InternalState;

    // Free modern keyboard tracking states
    xkb_state_unref(State->XkbState);
    xkb_keymap_unref(State->XkbKeymap);
    xkb_context_unref(State->XkbContext);

    xcb_destroy_window(State->Connection, State->Window);
    xcb_disconnect(State->Connection); // Safely close the direct server socket
}

Bool8 PlatformPollEvents(PlatformState* Platform)
{
    InternalState* State = (InternalState*)Platform->InternalState;

    xcb_generic_event_t* Event = NULL;
    xcb_client_message_event_t* ClientMessage;

    Bool8 QuitFlagged = FALSE;

    // Poll for events until null is returned.
    while ((Event = xcb_poll_for_event(State->Connection)) != NULL) {
        
        if (Event == 0) 
        {
            break;
        }
    

    // Input events
    switch (Event->response_type & ~0x80)
    {
            case XCB_KEY_PRESS:
            case XCB_KEY_RELEASE: 
            {
                xcb_key_press_event_t* KbEvent = (xcb_key_press_event_t*)Event;
                Bool8 Pressed = (Event->response_type & ~0x80) == XCB_KEY_PRESS;
                xcb_keycode_t Code = KbEvent->detail;

                // Pure XCB modern string translation via libxkbcommon
                xkb_keysym_t KeySymbol = xkb_state_key_get_one_sym(State->XkbState, Code);

                Keys Key = TranslateKeyCode(KeySymbol);
                InputProcessKey(Key, Pressed);
            } break;
            case XCB_BUTTON_PRESS:
            case XCB_BUTTON_RELEASE: 
            {
                xcb_button_press_event_t* MouseEvent = (xcb_button_press_event_t*)Event;

                Bool8 Pressed = ((Event->response_type & ~0x80) == XCB_BUTTON_PRESS);
                MouseButtons MouseButton = ButtonCount;

                switch (MouseEvent->detail) {
                    case XCB_BUTTON_INDEX_1:
                        MouseButton = MouseButtonLeft;
                        break;
                    case XCB_BUTTON_INDEX_2:
                        MouseButton = MouseButtonMiddle;
                        break;
                    case XCB_BUTTON_INDEX_3:
                        MouseButton = MouseButtonRight;
                        break;
                    case XCB_BUTTON_INDEX_8:
                        MouseButton = MouseButtonBackwards;
                        break;
                    case XCB_BUTTON_INDEX_9:
                        MouseButton = MouseButtonForward;
                        break;
                    default:
                        break;
                }
            
                if (MouseButton != ButtonCount) {
                    InputProcessMouseButton(MouseButton, Pressed);
                }
            } break;
            case XCB_MOTION_NOTIFY:
                {
                    xcb_motion_notify_event_t* MoveEvent = (xcb_motion_notify_event_t*)Event;

                    InputProcessMouseMove(MoveEvent->event_x, MoveEvent->event_y);
                    break;
                }

            case XCB_CONFIGURE_NOTIFY: {
                // TODO: Resizing
                break;
            }

            case XCB_CLIENT_MESSAGE: {
                ClientMessage = (xcb_client_message_event_t*)Event;

                // Window close
                if (ClientMessage->data.data32[0] == State->WmDeleteWin) {
                    QuitFlagged = TRUE;
                }
            } break;
            default:
                // Something else
                break;
        } 

        free(Event);
    }

    return !QuitFlagged;
}

void* PlatformAllocate(UInt64 Size, Bool8 Aligned)
{
    (void)Aligned;
    return malloc(Size);
}

void PlatformFree(void* Block, Bool8 Aligned)
{
    (void)Aligned;
    free(Block);
}

void* PlatformZeroMemory(void* Block, UInt64 Size)
{
    return memset(Block, 0, Size);
}
void* PlatformCopyMemory(void* Dest, const void* Source, UInt64 Size)
{
    return memcpy(Dest, Source, Size);
}

void* PlatformSetMemory(void* Dest, Int32 Value, UInt64 Size)
{
    return memset(Dest, Value, Size);
}

void PlatformConsoleWrite(const char* Message, UInt8 Color)
{
    // FATAL,ERROR,WARN,INFO,DEBUG,TRACE
    const char* ColorStrings[] = {"1;31", "0;31", "1;33", "1;37", "1;32", "1;36"};
    printf("\033[%sm%s\033[0m", ColorStrings[Color], Message);
}

void PlatformConsoleWriteError(const char* Message, UInt8 Color)
{
    // FATAL,ERROR,WARN,INFO,DEBUG,TRACE
    const char* ColorStrings[] = {"1;31", "0;31", "1;33", "1;37", "1;32", "1;36"};
    printf("\033[%sm%s\033[0m", ColorStrings[Color], Message);
}

Float64 PlatformGetAbsoluteTime()
{
    struct timespec Current;
    clock_gettime(CLOCK_MONOTONIC, &Current);
    return Current.tv_sec + Current.tv_nsec * 1e-9;
}

void PlatformSleep(UInt64 Miliseconds)
{
#if _POSIX_C_SOURCE >= 199309L
    struct timespec SleepTime;
    SleepTime.tv_sec = Miliseconds / 1000;
    SleepTime.tv_nsec = (Miliseconds % 1000) * 1000000;
    nanosleep(&SleepTime, 0);
#else
    if (Miliseconds >= 1000) {
        sleep(Miliseconds / 1000);
    }
    usleep((Miliseconds % 1000) * 1000);
#endif
}

Keys TranslateKeyCode(xkb_keysym_t XKeycode)
{
    switch (XKeycode)
    {
        case XKB_KEY_BackSpace:   return BackspaceKey;
        case XKB_KEY_Return:      return EnterKey;
        case XKB_KEY_Tab:         return TabKey;
        case XKB_KEY_Escape:      return EscapeKey;
        case XKB_KEY_Pause:       return PauseKey;
        case XKB_KEY_Caps_Lock:   return CapitalKey;
        case XKB_KEY_space:       return SpaceKey;
        case XKB_KEY_Page_Up:     return PriorKey;
        case XKB_KEY_Page_Down:   return NextKey;
        case XKB_KEY_End:         return EndKey;
        case XKB_KEY_Home:        return HomeKey;
        case XKB_KEY_Left:        return LeftKey;
        case XKB_KEY_Up:          return UpKey;
        case XKB_KEY_Right:       return RightKey;
        case XKB_KEY_Down:        return DownKey;
        case XKB_KEY_Select:      return SelectKey;
        case XKB_KEY_Print:       return PrintKey;
        case XKB_KEY_Execute:     return ExecuteKey;
        case XKB_KEY_Insert:      return InsertKey;
        case XKB_KEY_Delete:      return DeleteKey;
        case XKB_KEY_Help:        return HelpKey;

        // --- SPECIAL MODIFIER SYSTEM KEYS ---
        case XKB_KEY_Shift_L:     return LShiftKey;
        case XKB_KEY_Shift_R:     return RShiftKey;
        case XKB_KEY_Control_L:   return LControlKey;
        case XKB_KEY_Control_R:   return RControlKey;
        case XKB_KEY_Alt_L:       return LMenuKey;
        case XKB_KEY_Alt_R:       return RMenuKey;
        case XKB_KEY_Super_L:     return LWinKey;
        case XKB_KEY_Super_R:     return RWinKey;
        case XKB_KEY_Menu:        return AppsKey;
        case XKB_KEY_Sleep:       return SleepKey;

        // --- INTERNATIONAL LANGUAGE INPUT MAPS ---
        case XKB_KEY_Henkan:      return ConvertKey;
        case XKB_KEY_Muhenkan:    return NonConvertKey;
        case XKB_KEY_Kanji:       return AcceptKey;
        case XKB_KEY_Mode_switch: return ModeChangeKey;

        // --- MAIN NUMBER KEYS (0-9 Above Letters) ---
        case XKB_KEY_0: return Num0Key;
        case XKB_KEY_1: return Num1Key;
        case XKB_KEY_2: return Num2Key;
        case XKB_KEY_3: return Num3Key;
        case XKB_KEY_4: return Num4Key;
        case XKB_KEY_5: return Num5Key;
        case XKB_KEY_6: return Num6Key;
        case XKB_KEY_7: return Num7Key;
        case XKB_KEY_8: return Num8Key;
        case XKB_KEY_9: return Num9Key;

        // --- ALPHABETICAL CHARACTER KEYS (Case Insensitive Fallthroughs) ---
        case XKB_KEY_a: case XKB_KEY_A: return AKey;
        case XKB_KEY_b: case XKB_KEY_B: return BKey;
        case XKB_KEY_c: case XKB_KEY_C: return CKey;
        case XKB_KEY_d: case XKB_KEY_D: return DKey;
        case XKB_KEY_e: case XKB_KEY_E: return EKey;
        case XKB_KEY_f: case XKB_KEY_F: return FKey;
        case XKB_KEY_g: case XKB_KEY_G: return GKey;
        case XKB_KEY_h: case XKB_KEY_H: return HKey;
        case XKB_KEY_i: case XKB_KEY_I: return IKey;
        case XKB_KEY_j: case XKB_KEY_J: return JKey;
        case XKB_KEY_k: case XKB_KEY_K: return KKey;
        case XKB_KEY_l: case XKB_KEY_L: return LKey;
        case XKB_KEY_m: case XKB_KEY_M: return MKey;
        case XKB_KEY_n: case XKB_KEY_N: return NKey;
        case XKB_KEY_o: case XKB_KEY_O: return OKey;
        case XKB_KEY_p: case XKB_KEY_P: return PKey;
        case XKB_KEY_q: case XKB_KEY_Q: return QKey;
        case XKB_KEY_r: case XKB_KEY_R: return RKey;
        case XKB_KEY_s: case XKB_KEY_S: return SKey;
        case XKB_KEY_t: case XKB_KEY_T: return TKey;
        case XKB_KEY_u: case XKB_KEY_U: return UKey;
        case XKB_KEY_v: case XKB_KEY_V: return VKey;
        case XKB_KEY_w: case XKB_KEY_W: return WKey;
        case XKB_KEY_x: case XKB_KEY_X: return XKey;
        case XKB_KEY_y: case XKB_KEY_Y: return YKey;
        case XKB_KEY_z: case XKB_KEY_Z: return ZKey;

        // --- NUMERIC KEYPAD MODIFIERS AND AXIS FIELDS ---
        case XKB_KEY_KP_0:        return Numpad0Key;
        case XKB_KEY_KP_1:        return Numpad1Key;
        case XKB_KEY_KP_2:        return Numpad2Key;
        case XKB_KEY_KP_3:        return Numpad3Key;
        case XKB_KEY_KP_4:        return Numpad4Key;
        case XKB_KEY_KP_5:        return Numpad5Key;
        case XKB_KEY_KP_6:        return Numpad6Key;
        case XKB_KEY_KP_7:        return Numpad7Key;
        case XKB_KEY_KP_8:        return Numpad8Key;
        case XKB_KEY_KP_9:        return Numpad9Key;
        case XKB_KEY_KP_Multiply: return MultiplyKey;
        case XKB_KEY_KP_Add:      return AddKey;
        case XKB_KEY_KP_Separator:return SeparatorKey;
        case XKB_KEY_KP_Subtract: return SubtractKey;
        case XKB_KEY_KP_Decimal:  return DecimalKey;
        case XKB_KEY_KP_Divide:   return DivideKey;
        case XKB_KEY_KP_Equal:    return NumpadEqualKey;
        case XKB_KEY_Num_Lock:    return NumLockKey;
        case XKB_KEY_Scroll_Lock: return ScrollKey;

        // --- FUNCTION EXTENSION KEYS ---
        case XKB_KEY_F1:  return F1Key;
        case XKB_KEY_F2:  return F2Key;
        case XKB_KEY_F3:  return F3Key;
        case XKB_KEY_F4:  return F4Key;
        case XKB_KEY_F5:  return F5Key;
        case XKB_KEY_F6:  return F6Key;
        case XKB_KEY_F7:  return F7Key;
        case XKB_KEY_F8:  return F8Key;
        case XKB_KEY_F9:  return F9Key;
        case XKB_KEY_F10: return F10Key;
        case XKB_KEY_F11: return F11Key;
        case XKB_KEY_F12: return F12Key;
        case XKB_KEY_F13: return F13Key;
        case XKB_KEY_F14: return F14Key;
        case XKB_KEY_F15: return F15Key;
        case XKB_KEY_F16: return F16Key;
        case XKB_KEY_F17: return F17Key;
        case XKB_KEY_F18: return F18Key;
        case XKB_KEY_F19: return F19Key;
        case XKB_KEY_F20: return F20Key;
        case XKB_KEY_F21: return F21Key;
        case XKB_KEY_F22: return F22Key;
        case XKB_KEY_F23: return F23Key;
        case XKB_KEY_F24: return F24Key;

        // --- PUNCTUATION AND SYMBOLS ---
        case XKB_KEY_semicolon: case XKB_KEY_colon:     return SemicolonKey;
        case XKB_KEY_plus:      case XKB_KEY_equal:     return PlusKey;
        case XKB_KEY_comma:     case XKB_KEY_less:      return CommaKey;
        case XKB_KEY_minus:     case XKB_KEY_underscore: return MinusKey;
        case XKB_KEY_period:    case XKB_KEY_greater:   return PeriodKey;
        case XKB_KEY_slash:     case XKB_KEY_question:  return SlashKey;
        case XKB_KEY_grave:     case XKB_KEY_asciitilde: return GraveKey;
        case XKB_KEY_bracketleft:  case XKB_KEY_braceleft:  return LBracketKey;
        case XKB_KEY_backslash:    case XKB_KEY_bar:        return BackslashKey;
        case XKB_KEY_bracketright: case XKB_KEY_braceright: return RBracketKey;
        case XKB_KEY_apostrophe:   case XKB_KEY_quotedbl:   return QuoteKey;

        default:
            return 0;
    }
}

#endif