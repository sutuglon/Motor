/*
     File: OpenGLRenderer.m
 Abstract: The OpenGLRenderer class creates and draws the shaders.
  Version: 1.0
 
 Disclaimer: IMPORTANT:  This Apple software is supplied to you by Apple
 Inc. ("Apple") in consideration of your agreement to the following
 terms, and your use, installation, modification or redistribution of
 this Apple software constitutes acceptance of these terms.  If you do
 not agree with these terms, please do not use, install, modify or
 redistribute this Apple software.
 
 In consideration of your agreement to abide by the following terms, and
 subject to these terms, Apple grants you a personal, non-exclusive
 license, under Apple's copyrights in this original Apple software (the
 "Apple Software"), to use, reproduce, modify and redistribute the Apple
 Software, with or without modifications, in source and/or binary forms;
 provided that if you redistribute the Apple Software in its entirety and
 without modifications, you must retain this notice and the following
 text and disclaimers in all such redistributions of the Apple Software.
 Neither the name, trademarks, service marks or logos of Apple Inc. may
 be used to endorse or promote products derived from the Apple Software
 without specific prior written permission from Apple.  Except as
 expressly stated in this notice, no other rights or licenses, express or
 implied, are granted by Apple herein, including but not limited to any
 patent rights that may be infringed by your derivative works or by other
 works in which the Apple Software may be incorporated.
 
 The Apple Software is provided by Apple on an "AS IS" basis.  APPLE
 MAKES NO WARRANTIES, EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION
 THE IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY AND FITNESS
 FOR A PARTICULAR PURPOSE, REGARDING THE APPLE SOFTWARE OR ITS USE AND
 OPERATION ALONE OR IN COMBINATION WITH YOUR PRODUCTS.
 
 IN NO EVENT SHALL APPLE BE LIABLE FOR ANY SPECIAL, INDIRECT, INCIDENTAL
 OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 INTERRUPTION) ARISING IN ANY WAY OUT OF THE USE, REPRODUCTION,
 MODIFICATION AND/OR DISTRIBUTION OF THE APPLE SOFTWARE, HOWEVER CAUSED
 AND WHETHER UNDER THEORY OF CONTRACT, TORT (INCLUDING NEGLIGENCE),
 STRICT LIABILITY OR OTHERWISE, EVEN IF APPLE HAS BEEN ADVISED OF THE
 POSSIBILITY OF SUCH DAMAGE.
 
 Copyright (C) 2010~2011 Apple Inc. All Rights Reserved.
 
 */

#import "OpenGLRenderer.h"

#import "vec.h"
#import "shader.h"
#import "readimg.h"
#import "texture.h"
#import "objreader.h"
#import "object.h"
#import "FBO.h"
#import "chemain.h"
#import "bundle.h"

namespace  {
    FBO fbo;
    EAGLContext * _context;
    RunData runData;
}



@implementation OpenGLRenderer

@synthesize slider0Value;

- (BOOL) resizeFromLayer:(CAEAGLLayer *)layer
//- (void) resizeWithWidth:(GLuint)width AndHeight:(GLuint)height
{
    fbo.bind();
    [_context renderbufferStorage:GL_RENDERBUFFER fromDrawable:layer];
    UInt2 backingSize = getRenderBufferSize();
    fbo.setSize(backingSize);
    fbo.attach();
    
    ReshapeHandler(backingSize[FBO::eWidth], backingSize[FBO::eHeight]);
    
	return YES;
}

- (void) runWithLow:(Point3)lowAcc AndHi:(Point3)hiAcc AndPos:(CGPoint)pos AndDeltaPos:(CGPoint) deltaPos AndDeltaDistance:(CGFloat)deltaDistance 
            AndType:(int)touchType AndFingers:(int)touchFingers AndTouchTaps:(int)touchTaps
{
    runData.lowAcc_ = Real3(lowAcc.x,lowAcc.y,lowAcc.z);
    runData.hiAcc_ = Real3(hiAcc.x,hiAcc.y,hiAcc.z);
    runData.pos_ = Real2(pos.x,pos.y);
    runData.deltaPos_ = Real2(deltaPos.x,deltaPos.y);
    runData.touchType_ = touchType;
    runData.touchFingers_ = touchFingers;
    runData.touchTaps_ = touchTaps;
    runData.deltaDistance_ = deltaDistance;
    runData.slider0Value = slider0Value;

    IdleHandler(runData);
}

- (void) render
{

    [EAGLContext setCurrentContext:_context];

    fbo.bind();
    DisplayHandler();
    
//    glFinish();

    [_context presentRenderbuffer:GL_RENDERBUFFER];
}

- (id) initWithContext:(EAGLContext *) context AndDrawable:(id<EAGLDrawable>)drawable
//- (id) initWithDefaultFBO: (GLuint) defaultFBOName
{
     _context = context;
    
	if((self = [super init]))
	{
        fbo.initToFramebuffer();
        [_context renderbufferStorage:GL_RENDERBUFFER fromDrawable:drawable];
        UInt2 backingSize = getRenderBufferSize();
        fbo.setSize(backingSize);
        fbo.attach();

        Init(backingSize[FBO::eWidth], backingSize[FBO::eHeight]);
        
        // This is done in order to pre-warm OpenGL
        [self render];

		GetGLError();
	}
    
    slider0Value = 0.75;
	return self;
}


- (void) dealloc
{
	// Cleanup all OpenGL 
    fbo.destroy();
    Cleanup();
	// TODO: glDeleteTextures(1, &_characterTexName);
	[super dealloc];	
}

@end
