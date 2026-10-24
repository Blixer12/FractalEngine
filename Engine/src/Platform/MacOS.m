#include "Platform/Platform.h"

#ifdef(FPLATFORM_APPLE)

#include "Core/Logger.h"
#include "Core/Event.h"
#include "Core/Input.h"

#include "Containers/Vector.h"

#include <mach/mach_time.h>
#include <crt_externs.h>

#import <Foundation/Foundation.h>
#import <Cocoa/Cocoa.h>
#import <QuartzCore/QuartzCore.h>

// For surface creation
#define VK_USE_PLATFORM_METAL_EXT
#include <vulkan/vulkan.h>
#include "Renderer/Vulkan/VulkanDef.inl"

@class ApplicationDelegate;
@class WindowDelegate;
@class ContentView;

typedef struct PlatformState {
    ApplicationDelegate* AppDelegate;
    WindowDelegate* WndDelegate;
    NSWindow* Window;
    ContentView* View;
    CAMetalLayer* Layer;
    VkSurfaceKHR Surface;
    Bool8 QuitFlagged;
} PlatformState;

static PlatformState* StatePtr;

// Key Translation
Keys TranslateKeycode(UInt32 NSKeycode);

@interface ContentView : NSView <NSTextInputClient> {
    NSWindow* Window;
    NSTrackingArea* TrackingArea;
    NSMutableAttributedString* MarkedText;
}

- (instancetype)initWithWindow:(NSWindow*)InitWindow;

@end // ContentView

@implementation ContentView

- (instancetype)initWithWindow:(NSWindow*)InitWindow {
    self = [super init];
    if (self != nil) {
        Window = InitWindow;
    }
    return self;
}

- (BOOL)canBecomeKeyView {
    return YES;
}

- (BOOL)acceptsFirstResponder {
    return YES;
}

- (BOOL)wantsUpdateLayer {
    return YES;
}

- (BOOL)acceptsFirstMouse:(NSEvent *)event {
    return YES;
}

- (void)mouseDown:(NSEvent *)event {
    InputProcessMouseButton(MouseButtonLeft, true);
}

- (void)mouseDragged:(NSEvent *)event {
    [self mouseMoved:event];
}

- (void)mouseUp:(NSEvent *)event {
    InputProcessMouseButton(MouseButtonLeft, false);
}

- (void)mouseMoved:(NSEvent *)event {
    const NSPoint pos = [event locationInWindow];
    InputProcessMouseMove((Int16)pos.x, (Int16)pos.y);
}

- (void)rightMouseDown:(NSEvent *)event {
    InputProcessMouseButton(MouseButtonRight, true);
}

- (void)rightMouseDragged:(NSEvent *)event {
    [self mouseMoved:event];
}

- (void)rightMouseUp:(NSEvent *)event {
    InputProcessMouseButton(MouseButtonRight, false);
}

- (void)otherMouseDown:(NSEvent *)event {
    InputProcessMouseButton(MouseButtonMiddle, true);
}

- (void)otherMouseDragged:(NSEvent *)event {
    [self mouseMoved:event];
}

- (void)otherMouseUp:(NSEvent *)event {
    InputProcessMouseButton(MouseButtonMiddle, false);
}

- (void)keyDown:(NSEvent *)event {
    Keys Key = TranslateKeycode((UInt32)[event keyCode]);
    InputProcessKey(Key, true);
    [self interpretKeyEvents:@[event]];
}

- (void)keyUp:(NSEvent *)event {
    Keys Key = TranslateKeycode((UInt32)[event keyCode]);
    InputProcessKey(Key, false);
}

- (void)scrollWheel:(NSEvent *)event {
    InputProcessMouseWheel((Int8)[event scrollingDeltaY]);
}

- (void)insertText:(id)string replacementRange:(NSRange)replacementRange {}
- (void)setMarkedText:(id)string selectedRange:(NSRange)selectedRange replacementRange:(NSRange)replacementRange {}
- (void)unmarkText {}

static const NSRange KEmptyRange = { NSNotFound, 0 };

- (NSRange)selectedRange { return KEmptyRange; }
- (NSRange)markedRange { return KEmptyRange; }
- (BOOL)hasMarkedText { return false; }
- (nullable NSAttributedString *)attributedSubstringForProposedRange:(NSRange)range actualRange:(nullable NSRangePointer)actualRange { return nil; }
- (NSArray<NSAttributedStringKey> *)validAttributesForMarkedText { return [NSArray array]; }
- (NSRect)firstRectForCharacterRange:(NSRange)range actualRange:(nullable NSRangePointer)actualRange { return NSMakeRect(0, 0, 0, 0); }
- (NSUInteger)characterIndexForPoint:(NSPoint)point { return 0; }

@end // ContentView

@interface ApplicationDelegate : NSObject <NSApplicationDelegate> {}

@end // ApplicationDelegate

@implementation ApplicationDelegate

- (void)applicationDidFinishLaunching:(NSNotification *)notification {
    @autoreleasepool {
        NSEvent* event = [NSEvent otherEventWithType:NSEventTypeApplicationDefined
                                            location:NSMakePoint(0, 0)
                                       modifierFlags:0
                                           timestamp:0
                                        windowNumber:0
                                             context:nil
                                             subtype:0
                                               data1:0
                                               data2:0];
        [NSApp postEvent:event atStart:YES];
    }
    [NSApp stop:nil];
}

@end // ApplicationDelegate

Bool8 PlatformSystemStartup(
    UInt64* MemoryRequirement,
    void* State,
    const char* ApplicationName,
    Int32 X,
    Int32 Y,
    Int32 Width,
    Int32 Height) 
{
    *MemoryRequirement = sizeof(PlatformState);
    if (State == 0) {
        return true;
    }

    StatePtr = State;

    @autoreleasepool {
        [NSApplication sharedApplication];

        StatePtr->AppDelegate = [[ApplicationDelegate alloc] init];
        if (!StatePtr->AppDelegate) {
            FLERROR("Failed to create application delegate");
            return false;
        }
        [NSApp setDelegate:StatePtr->AppDelegate];

        StatePtr->WndDelegate = [[WindowDelegate alloc] initWithState:State];
        if (!StatePtr->WndDelegate) {
            FLERROR("Failed to create window delegate");
            return false;
        }

        StatePtr->Window = [[NSWindow alloc]
            initWithContentRect:NSMakeRect(X, Y, Width, Height)
            styleMask:NSWindowStyleMaskMiniaturizable|NSWindowStyleMaskTitled|NSWindowStyleMaskClosable|NSWindowStyleMaskResizable
            backing:NSBackingStoreBuffered
            defer:NO];
        if (!StatePtr->Window) {
            FLERROR("Failed to create window");
            return false;
        }

        StatePtr->Layer = [CAMetalLayer layer];
        if (!StatePtr->Layer) {
            FLERROR("Failed to create layer for view");
        }

        StatePtr->View = [[ContentView alloc] initWithWindow:StatePtr->Window];
        [StatePtr->View setLayer:StatePtr->Layer];
        [StatePtr->View setWantsLayer:YES];

        [StatePtr->Window setLevel:NSNormalWindowLevel];
        [StatePtr->Window setContentView:StatePtr->View];
        [StatePtr->Window makeFirstResponder:StatePtr->View];
        [StatePtr->Window setTitle:@(ApplicationName)];
        [StatePtr->Window setDelegate:StatePtr->WndDelegate];
        [StatePtr->Window setAcceptsMouseMovedEvents:YES];
        [StatePtr->Window setRestorable:NO];

        if (![[NSRunningApplication currentApplication] isFinishedLaunching]) {
            [NSApp run];
        }

        [NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];
        [NSApp activateIgnoringOtherApps:YES];
        [StatePtr->Window makeKeyAndOrderFront:nil];

        return true;
    }
}

void PlatformSystemShutdown(void* PlatformStateParam) {
    if (StatePtr) {
        @autoreleasepool {
            [StatePtr->Window orderOut:nil];
            [StatePtr->Window setDelegate:nil];
            [StatePtr->WndDelegate release];

            [StatePtr->View release];
            StatePtr->View = nil;

            [StatePtr->Window close];
            StatePtr->Window = nil;

            [NSApp setDelegate:nil];
            [StatePtr->AppDelegate release];
            StatePtr->AppDelegate = nil;
        }
    }
    StatePtr = 0;
}

Bool8 PlatformPollEvents() {
    if (StatePtr) {
        @autoreleasepool {
            NSEvent* Event;
            for (;;) {
                Event = [NSApp nextEventMatchingMask:NSEventMaskAny
                                          untilDate:[NSDate distantPast]
                                             inMode:NSDefaultRunLoopMode
                                            dequeue:YES];
                if (!Event) break;
                [NSApp sendEvent:Event];
            }
        }
        return !StatePtr->QuitFlagged;
    }
    return true;
}

void* PlatformAllocate(UInt64 Size, Bool8 Aligned) {
    return malloc(Size);
}

void PlatformFree(void* Block, Bool8 Aligned) {
    free(Block);
}

void* PlatformZeroMemory(void* Block, UInt64 Size) {
    return memset(Block, 0, Size);
}

void* PlatformCopyMemory(void* Dest, const void* Source, UInt64 Size) {
    return memcpy(Dest, Source, Size);
}

void* PlatformSetMemory(void* Dest, Int32 Value, UInt64 Size) {
    return memset(Dest, Value, Size);
}

void PlatformConsoleWrite(const char* Message, UInt8 Color) {
    const char* ColourStrings[] = {"0;41", "1;31", "1;33", "1;32", "1;34", "1;30"};
    printf("\033[%sm%s\033[0m", ColourStrings[Color], Message);
}

void PlatformConsoleWriteError(const char* Message, UInt8 Color) {
    const char* ColourStrings[] = {"0;41", "1;31", "1;33", "1;32", "1;34", "1;30"};
    printf("\033[%sm%s\033[0m", ColourStrings[Color], Message);
}

Float64 PlatformGetAbsoluteTime() {
    return mach_absolute_time();
}

void PlatformSleep(UInt64 Ms) {
#if _POSIX_C_SOURCE >= 199309L
    struct timespec Ts;
    Ts.tv_sec = Ms / 1000;
    Ts.tv_nsec = (Ms % 1000) * 1000 * 1000;
    nanosleep(&Ts, 0);
#else
    if (Ms >= 1000) {
        sleep(Ms / 1000);
    }
    usleep((Ms % 1000) * 1000);
#endif
}

Bool8 PlatformCreateVulkanSurface(VulkanContext* Context) {
    if (!StatePtr) {
        return false;
    }

    VkMetalSurfaceCreateInfoEXT CreateInfo = {0};
    CreateInfo.sType = VK_STRUCTURE_TYPE_METAL_SURFACE_CREATE_INFO_EXT;
    CreateInfo.pLayer = StatePtr->Layer;

    VkResult Result = vkCreateMetalSurfaceEXT(
        Context->Instance, 
        &CreateInfo,
        Context->Allocator,
        &StatePtr->Surface);
    if (Result != VK_SUCCESS) {
        FLFATAL("Vulkan surface creation failed.");
        return false;
    }

    Context->Surface = StatePtr->Surface;
    return true;
}

Keys TranslateKeycode(UInt32 NSKeycode) { 
    switch (NSKeycode) {
        // Alphabet
        case 0x00: return AKey;
        case 0x0B: return BKey;
        case 0x08: return CKey;
        case 0x02: return DKey;
        case 0x0E: return EKey;
        case 0x03: return FKey;
        case 0x05: return GKey;
        case 0x04: return HKey;
        case 0x22: return IKey;
        case 0x26: return JKey;
        case 0x28: return KKey;
        case 0x25: return LKey;
        case 0x2E: return MKey;
        case 0x2D: return NKey;
        case 0x1F: return OKey;
        case 0x23: return PKey;
        case 0x0C: return QKey;
        case 0x0F: return RKey;
        case 0x01: return SKey;
        case 0x11: return TKey;
        case 0x20: return UKey;
        case 0x09: return VKey;
        case 0x0D: return WKey;
        case 0x07: return XKey;
        case 0x10: return YKey;
        case 0x06: return ZKey;

        // Special & Navigation Keys
        case 0x33: return BackspaceKey;
        case 0x39: return CapitalKey;
        case 0x75: return DeleteKey;
        case 0x7D: return DownKey;
        case 0x24: return EnterKey;
        case 0x35: return EscapeKey;
        case 0x7B: return LeftKey;
        case 0x7C: return RightKey;
        case 0x31: return SpaceKey;
        case 0x30: return TabKey;
        case 0x7E: return UpKey;

        default: return KeysCount;
    }
}
#endif // FPLATFORM_APPLE