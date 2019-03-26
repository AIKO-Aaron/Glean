//
//  Mac_Window.m
//  Glean
//
//  Created by Aaron Hodel on 24.03.19.
//  Copyright © 2019 Aaron Hodel. All rights reserved.
//

#ifdef __APPLE__

#import <Foundation/Foundation.h>
#import <Cocoa/Cocoa.h>

#include "Window.h"

@interface WindowDelegate : NSObject<NSWindowDelegate>

@end

@implementation WindowDelegate
{
    graphics::Window *window;
}

-(id) initWithWindow: (graphics::Window*) iWindow {
    if(self = [super init]) window = iWindow;
    return self;
}

- (void)windowWillClose:(NSNotification *)notification {
    printf("Window will close now...\n");
    window->running = false;
}

- (void)windowDidBecomeKey:(NSNotification *)notification {
    printf("Window is now focused\n");
}

@end

@interface AppDelegate : NSObject<NSApplicationDelegate>

@end

@implementation AppDelegate
{
    graphics::Window *window;
}

- (instancetype)init: (graphics::Window*) w
{
    self = [super init];
    if (self) {
        window = w;
    }
    return self;
}

-(void) onQuit:(__unused id) sender {
    [[NSApplication sharedApplication] terminate:nil];
}

- (void)applicationDidFinishLaunching:(NSNotification *)notification {
    if(!window) printf("Window = NULL");
    printf("Hello world! The application is running now\n");
    window->loop();
}

- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)sender {
    return YES;
}

@end

using namespace graphics;

Window::Window() {
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
    [NSApplication sharedApplication];
    [NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];
    
    [NSApp activateIgnoringOtherApps:YES];
    [NSApp setDelegate:[[AppDelegate alloc] init: this]];

    NSScreen *screen = [NSScreen mainScreen];
    CGSize windowSize;
    windowSize.width = 960;
    windowSize.height = 540;
    
    NSRect frame = NSMakeRect((screen.frame.size.width - windowSize.width) / 2.0f, (screen.frame.size.height - windowSize.height) / 2.0f, windowSize.width, windowSize.height);
    NSWindowStyleMask windowStyleMask = NSWindowStyleMaskTitled | NSWindowStyleMaskClosable | NSWindowStyleMaskMiniaturizable | NSWindowStyleMaskResizable;
    NSWindow *window = [[NSWindow alloc] initWithContentRect:frame styleMask: windowStyleMask backing:NSBackingStoreBuffered defer: NO screen: screen];
    [window autorelease];
    
    this->window = (void*) window;
    
    [window setReleasedWhenClosed:NO];
    window.acceptsMouseMovedEvents = YES;
    WindowDelegate *windowDelegate = [[WindowDelegate alloc] initWithWindow:this];
    window.delegate = windowDelegate;
    window.opaque = false;
    window.level = 1;
    
    const char* title = "Hello World";
    [window setTitle:static_cast<NSString* _Nonnull>([NSString stringWithUTF8String:title])];
    
    [window setCollectionBehavior:NSWindowCollectionBehaviorFullScreenAuxiliary];
    
    [window orderFrontRegardless];
    [window makeKeyWindow];

    init();    
}

bool Window::fetchEvents() {
    NSWindow *window = (NSWindow*) this->window;
    NSEvent *evnt;
    do {
        evnt = [window nextEventMatchingMask:NSEventMaskAny untilDate:[NSDate distantPast] inMode:NSDefaultRunLoopMode dequeue:YES];
        if(evnt) [NSApp sendEvent: evnt];
    } while(evnt);
    
    return true;
}

void Window::start() { [NSApp run]; }

#endif
