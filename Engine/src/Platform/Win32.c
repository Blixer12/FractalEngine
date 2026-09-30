#include "Platform.h"

//Windows Platform
#if FPLATFORM_WINDOWS

    #include "Core/Logger.h"
    #include "Core/Input.h"
    #include "Core/Event.h"
    #include "Containers/Vector.h"

    #define WIN32_LEAN_AND_MEAN
    
    #define NOMINMAX
    #define NOIME
    #define NOSERVICE

    #define NOGDI
    #define NOMENUS
    #define NOHELP
    #define NOPROFILER
    #define NODRAWTEXT

    #include <Windows.h>
    #include <Windowsx.h> 
    #include <stdlib.h>
    #include <stdio.h>

    #define VK_USE_PLATFORM_WIN32_KHR
    #include "Renderer/Vulkan/VulkanDef.inl"

typedef struct InternalState{
    HINSTANCE AppInstance;
    HWND MainWindow;
    VkSurfaceKHR Surface;
} InternalState;

//Clock
static Float64 ClockFrequency;
static UInt64 StartTime;

LRESULT CALLBACK Win32ProcessMessage(HWND HWindow, UInt32 Message, WPARAM WordParam, LPARAM LongParam);

Bool8 PlatformStartup( PlatformState* Platform, const char* WindowName, Int32 X, Int32 Y, Int32 Width, Int32 Height)
{
    Platform->InternalState = malloc(sizeof(InternalState));
    InternalState* State = (InternalState* )Platform->InternalState;

    State->AppInstance = GetModuleHandleA(0);

HICON WindowIcon = LoadIcon(State->AppInstance, IDI_APPLICATION);
WNDCLASSA WindowClass = {0};

WindowClass.style = CS_DBLCLKS;  
WindowClass.lpfnWndProc = Win32ProcessMessage;
WindowClass.cbClsExtra = 0;
WindowClass.cbWndExtra = 0;
WindowClass.hInstance = State->AppInstance;
WindowClass.hIcon = WindowIcon;
WindowClass.hCursor = LoadCursor(NULL, IDC_ARROW);  
WindowClass.hbrBackground = NULL;                  
WindowClass.lpszClassName = "FractalWindowClass";

if (!RegisterClassA(&WindowClass))
{
    MessageBoxA(0, "Window Failed to Register", "Error!", MB_ICONEXCLAMATION | MB_OK);
    return false;
}

UInt32 ClientX = X;
UInt32 ClientY = Y;
UInt32 ClientWidth = Width;
UInt32 ClientHeight = Height;

UInt32 WindowX = ClientX;
UInt32 WindowY = ClientY;
UInt32 WindowWidth = ClientWidth;
UInt32 WindowHeight = ClientHeight;

UInt32 WindowStyle = WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION;
UInt32 WindowExStyle = WS_EX_APPWINDOW;

WindowStyle |= WS_MAXIMIZEBOX;
WindowStyle |= WS_MINIMIZEBOX;
WindowStyle |= WS_THICKFRAME;

RECT Border = {0, 0, 0, 0};
AdjustWindowRectEx(&Border, WindowStyle, false, WindowExStyle);

WindowWidth += (Border.right - Border.left);
WindowHeight += (Border.bottom - Border.top);

    HWND Handle = CreateWindowExA(
        WindowExStyle, "FractalWindowClass", WindowName,
        WindowStyle, WindowX, WindowY, WindowWidth, WindowHeight,
        0, 0, State->AppInstance, 0);

if (Handle == 0) {

    MessageBoxA(NULL, "Wndow Failed to Create", "Error!", MB_ICONEXCLAMATION | MB_OK);

    FLFATAL("Window creation failed!");
    return false;

} else {

    State->MainWindow = Handle;

}

Bool8 ShouldActivate = true; // TODO: if the window should not accept input, this should be false.
Int32 ShowWindowCommandFlags = ShouldActivate ? SW_SHOW : SW_SHOWNOACTIVATE;

// If initially minimized, use SW_MINIMIZE : SW_SHOWMINNOACTIVE;
// If initially maximized, use SW_SHOWMAXIMIZED : SW_MAXIMIZE
ShowWindow(State->MainWindow, ShowWindowCommandFlags);

LARGE_INTEGER Frequency;
QueryPerformanceFrequency(&Frequency);
ClockFrequency = 1.0 / (Float64)Frequency.QuadPart;

LARGE_INTEGER Counter;
QueryPerformanceCounter(&Counter);
StartTime = (UInt64)Counter.QuadPart;

return true;

}

void PlatformShutdown(PlatformState* Platform)
{
    InternalState* State = (InternalState* )Platform->InternalState;

    if (State->MainWindow)
    {
        DestroyWindow(State->MainWindow);
        State->MainWindow = 0;
    }
}

Bool8 PlatformPollEvents(PlatformState* Platform)
{
    (void)Platform;

    MSG Message;
    while (PeekMessageA(&Message, NULL, 0, 0, PM_REMOVE)) {
        TranslateMessage(&Message);
        DispatchMessageA(&Message);
    }

    return true;
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
    return memcpy (Dest, Source, Size);
}
void* PlatformSetMemory(void* Dest, Int32 Value, UInt64 Size)
{
    return memset(Dest, Value, Size);
}

void PlatformConsoleWrite(const char* Message, UInt8 Color)
{
    HANDLE ConsoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    // FATAL,ERROR,WARN,INFO,DEBUG,TRACE
    static UInt8 Levels[6] = {12, 4, 6, 7, 10, 11};
    SetConsoleTextAttribute(ConsoleHandle, Levels[Color]);

    static const char* AnsiColors[6] = {
        "\033[91m",      // 12 -> Bright Red (Fatal)
        "\033[31m",      // ERROR: Direct Bright Red
        "\033[93m",      // WARN:  Direct Bright Yellow
        "\033[37m",      // INFO:  Standard White
        "\033[92m",      // DEBUG: Direct Bright Green
        "\033[96m"       // TRACE: Direct Bright Cyan
    };

    enum { MessageLength = 32768 };
    static char DebuggerMessage[MessageLength];

    const char* LineStart = Message;
    const char* LineEnd = 0;

    while (*LineStart != '\0') {
        LineEnd = strchr(LineStart, '\n');
        if (LineEnd) {
            // Found a newline, print up to the newline with the coloring sequence attached
            int ComponentLength = (int)(LineEnd - LineStart);
            snprintf(DebuggerMessage, sizeof(DebuggerMessage), "%s%.*s\033[0m\n", AnsiColors[Color], ComponentLength, LineStart);
            OutputDebugStringA(DebuggerMessage);
            LineStart = LineEnd + 1; // Slide forward past the newline character
        } else {
            // No more newlines left, print the tail end residue of the string
            snprintf(DebuggerMessage, sizeof(DebuggerMessage), "%s%s\033[0m", AnsiColors[Color], LineStart);
            OutputDebugStringA(DebuggerMessage);
            break;
        }
    }

    UInt64 Length = strlen(Message);
    LPDWORD BytesWritten = 0;
    WriteConsoleA(GetStdHandle(STD_OUTPUT_HANDLE), Message, (DWORD)Length, BytesWritten, 0);
}

void PlatformConsoleWriteError(const char* Message, UInt8 Color)
{
    HANDLE ConsoleHandle = GetStdHandle(STD_ERROR_HANDLE);
    // FATAL,ERROR,WARN,INFO,DEBUG,TRACE
    static UInt8 Levels[6] = {12, 4, 6, 7, 10, 11};
    SetConsoleTextAttribute(ConsoleHandle, Levels[Color]);

    static const char* AnsiColors[6] = {
        "\033[91m", // 12 -> Bright Red (Fatal)
        "\033[31m",      // ERROR: Direct Bright Red
        "\033[93m",      // WARN:  Direct Bright Yellow
        "\033[37m",      // INFO:  Standard White
        "\033[92m",      // DEBUG: Direct Bright Green
        "\033[96m"       // TRACE: Direct Bright Cyan
    };

    enum { MessageLength = 32768 };
    static char DebuggerMessage[MessageLength];

    const char* LineStart = Message;
    const char* LineEnd = 0;

    while (*LineStart != '\0') {
        LineEnd = strchr(LineStart, '\n');
        if (LineEnd) {
            int ComponentLength = (int)(LineEnd - LineStart);
            snprintf(DebuggerMessage, sizeof(DebuggerMessage), "%s%.*s\033[0m\n", AnsiColors[Color], ComponentLength, LineStart);
            OutputDebugStringA(DebuggerMessage);
            LineStart = LineEnd + 1;
        } else {
            snprintf(DebuggerMessage, sizeof(DebuggerMessage), "%s%s\033[0m", AnsiColors[Color], LineStart);
            OutputDebugStringA(DebuggerMessage);
            break;
        }
    }

    UInt64 Length = strlen(Message);
    LPDWORD BytesWritten = 0;
    WriteConsoleA(GetStdHandle(STD_ERROR_HANDLE), Message, (DWORD)Length, BytesWritten, 0);
}

Float64 PlatformGetAbsoluteTime()
{
    LARGE_INTEGER CurrentTime;
    QueryPerformanceCounter(&CurrentTime);
    return (Float64)CurrentTime.QuadPart*  ClockFrequency;
}

void PlatformSleep(UInt64 Miliseconds)
{
    Sleep((DWORD)Miliseconds);
}



void PlatformGetRequiredExtensions(const char*** ExtensionsVector)
{
    VectorAppend(*ExtensionsVector, &"VK_KHR_win32_surface");
}

// Surface creation for Vulkan
Bool8 PlatformCreateVulkanSurface(PlatformState* Platform, VulkanContext *Context) {
    /// Simply cold-cast to the known type.
    InternalState* State = (InternalState*)Platform->InternalState;

    VkWin32SurfaceCreateInfoKHR CreateInfo = {0};
    CreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    CreateInfo.pNext = 0;
    CreateInfo.hinstance = State->AppInstance;
    CreateInfo.hwnd = State->MainWindow;

    VkResult result = vkCreateWin32SurfaceKHR(Context->Instance, &CreateInfo, Context->Allocator, &State->Surface);
    if (result != VK_SUCCESS) {
        FLFATAL("Vulkan surface creation failed.");
        return false;
    }

    Context->Surface = State->Surface;
    return true;
}

LRESULT CALLBACK Win32ProcessMessage(HWND HWindow, UInt32 Message, WPARAM WordParam, LPARAM LongParam)
{
    switch(Message)
    {
            case WM_ERASEBKGND:
            // Notify the OS that erasing will be handled by the application to prevent flicker.
            return 1; 

            case WM_CLOSE:
                EventContext Data = {0};
                EventFire(EVENT_APP_QUIT, 0, Data);
                return true;

            case WM_DESTROY:
            PostQuitMessage(0);
            return 0;

            case WM_SIZE:
            {
                // RECT ClientRect;
                // GetClientRect(HWindow, &ClientRect);
                // UInt32 Width = (UInt32)(ClientRect.right - ClientRect.left);
                // UInt32 Height = (UInt32)(ClientRect.bottom - ClientRect.top);

                //TODO: Fire a event to resize the window
            } break;

            case WM_KEYDOWN:
            case WM_SYSKEYDOWN:
            case WM_KEYUP:
            case WM_SYSKEYUP:
            {
                //Key Pressed/Released
                Bool8 Pressed = (Message == WM_KEYDOWN || Message == WM_SYSKEYDOWN);
                Keys Key = (UInt16)WordParam;
                
                InputProccessKey(Key, Pressed);

            } break;

            case WM_MOUSEMOVE:
            {
                // Mouse move
                Int32 XPosition = GET_X_LPARAM(LongParam);
                Int32 YPosition = GET_Y_LPARAM(LongParam);

                InputProcessMouseMove((Int16)XPosition, (Int16)YPosition);

            } break;

            case WM_MOUSEWHEEL:
            {
                Int32 WheelDelta = GET_WHEEL_DELTA_WPARAM(WordParam);
                if (WheelDelta != 0) 
                {
                    // Flatten the input to an OS-independent (-1, 1)
                    WheelDelta = (WheelDelta < 0) ? -1 : 1;
                    InputProcessMouseWheel((Int8)WheelDelta);
                }

            } break;

            case WM_LBUTTONDOWN:
            case WM_MBUTTONDOWN:
            case WM_RBUTTONDOWN:
            case WM_XBUTTONDOWN:
            case WM_LBUTTONUP:
            case WM_MBUTTONUP:
            case WM_RBUTTONUP:
            case WM_XBUTTONUP:
            {
                Bool8 Pressed = (Message == WM_LBUTTONDOWN || Message == WM_MBUTTONDOWN || Message == WM_RBUTTONDOWN || Message == WM_XBUTTONDOWN);
                MouseButtons MouseButton = ButtonCount;

                switch (Message)
                {
                    case WM_LBUTTONDOWN:
                    case WM_LBUTTONUP:
                        MouseButton = MouseButtonLeft;
                        break;

                    case WM_MBUTTONDOWN:
                    case WM_MBUTTONUP:
                        MouseButton = MouseButtonMiddle;
                        break;

                    case WM_RBUTTONDOWN:
                    case WM_RBUTTONUP:
                        MouseButton = MouseButtonRight;
                        break;
                    case WM_XBUTTONDOWN:
                    case WM_XBUTTONUP:
                    {
                        // XBUTTON1 is usually "Back", XBUTTON2 is usually "Forward"
                        UInt16 WindowsXButton = GET_XBUTTON_WPARAM(WordParam);
                        if (WindowsXButton == XBUTTON1) {
                            MouseButton = MouseButtonBackwards;
                        } else if (WindowsXButton == XBUTTON2) {
                            MouseButton = MouseButtonForward;
                        }
                        break;
                    }
                }

                if (MouseButton != ButtonCount)
                {
                    InputProcessMouseButton(MouseButton, Pressed);
                }

            } break;
    }

    return DefWindowProcA(HWindow, Message, WordParam, LongParam);
}

#endif //FPLATFORM_WINDOWS