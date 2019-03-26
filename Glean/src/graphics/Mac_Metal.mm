//
//  Mac_Metal.cpp
//  Glean
//
//  Created by Aaron Hodel on 26.03.19.
//  Copyright © 2019 Aaron Hodel. All rights reserved.
//
#ifdef __APPLE__

#import <Cocoa/Cocoa.h>
#import <Metal/Metal.h>
#import <MetalKit/MetalKit.h>

#define CMD_QUEUE ((id<MTLCommandQueue>) rd.commandQueue)
#define CMD_BUF ((id<MTLCommandBuffer>) rd.commandBuffer)
#define CMD_ENC ((id<MTLRenderCommandEncoder>) rd.commandEncoder)

@interface MetalView : MTKView
@end

@implementation MetalView {
        
}

- (void)drawInMTKView:(nonnull MTKView *)view {
    
}

@end

id<MTLDrawable> lastDrawn = nil;

#include "Renderer.h"
using namespace graphics;

Renderer::Renderer(__internalWindow window) {
    MTKView *mainView = [[MetalView alloc] init]; // Used to present I guess
    rd.window = window;
    
    // Setup view
    [mainView setDevice: MTLCreateSystemDefaultDevice()];
    [mainView setColorPixelFormat: MTLPixelFormatBGRA8Unorm];
    [mainView setClearColor: MTLClearColorMake(0, 0, 0, 1)];
    [mainView setDepthStencilPixelFormat: MTLPixelFormatDepth32Float_Stencil8];
    [mainView setSampleCount: 2];
    [mainView setEnableSetNeedsDisplay: YES];
    
    [(NSWindow*) window setContentView: mainView];

    rd.commandQueue = [[mainView device] newCommandQueue];
    rd.commandBuffer = [CMD_QUEUE commandBuffer];
    [(id<MTLCommandBuffer>) rd.commandBuffer presentDrawable: mainView.currentDrawable];
    synchronize();
}

void Renderer::swapBuffers() {
    MTKView *view = [((NSWindow*) rd.window) contentView];
    
    if(view.currentDrawable == lastDrawn) return;
    else lastDrawn = view.currentDrawable;
    
    [CMD_ENC endEncoding];
    [CMD_BUF presentDrawable: view.currentDrawable];
    [CMD_BUF commit];
    
    synchronize(); // Create new encoder
}

void Renderer::synchronize() { // Create new encoder
    MTKView *view = [((NSWindow*) rd.window) contentView];
    [view setNeedsDisplay: YES];
    
    rd.commandBuffer = [CMD_QUEUE commandBuffer];
    MTLRenderPassDescriptor* desc = [view currentRenderPassDescriptor];
    if(desc) rd.commandEncoder = [CMD_BUF renderCommandEncoderWithDescriptor:desc];
}

void Renderer::clearColor(float r, float g, float b, float a) {
    MTKView *view = [((NSWindow*) rd.window) contentView];
    [view setClearColor:MTLClearColorMake(r, g, b, a)];
}

#endif
