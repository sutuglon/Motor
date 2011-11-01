/*
     File: EAGLView.m
 Abstract: The EAGLView class is a UIView subclass that renders OpenGL scene.
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

#import "EAGLView.h"
//#import "ES2Renderer.h"
#import "OpenGLRenderer.h"

#import <iostream>
#import "chrono.h"



@implementation EAGLView

@synthesize animating;
@synthesize lowAcc;
@synthesize hiAcc;
@dynamic animationFrameInterval;



Chronometer& aniChrono = *Chronos::Singleton().add("aniChrono");



// You must implement this method
+ (Class) layerClass
{
    return [CAEAGLLayer class];
}

//The GL view is stored in the nib file. When it's unarchived it's sent -initWithCoder:
- (id) initWithCoder:(NSCoder*)coder
{    
    if ((self = [super initWithCoder:coder]))
	{
        // Get the layer
        CAEAGLLayer *eaglLayer = (CAEAGLLayer *)self.layer;
        
        eaglLayer.opaque = TRUE;
        eaglLayer.drawableProperties = [NSDictionary dictionaryWithObjectsAndKeys:
                                        [NSNumber numberWithBool:FALSE], kEAGLDrawablePropertyRetainedBacking, kEAGLColorFormatRGBA8, kEAGLDrawablePropertyColorFormat, nil];
		
		_context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
        
        if (!_context || ![EAGLContext setCurrentContext:_context]) {
			[self release];
			return nil;
		}
		
		renderer = [[OpenGLRenderer alloc] initWithContext:_context AndDrawable:(id<EAGLDrawable>)self.layer];
		if (!renderer) {
			[self release];
			return nil;
		}
        
		animating = FALSE;
		displayLinkSupported = FALSE;
		animationFrameInterval = 1;
		displayLink = nil;
		animationTimer = nil;
		
		// A system version of 3.1 or greater is required to use CADisplayLink. The NSTimer
		// class is used as fallback when it isn't available.
		NSString *reqSysVer = @"3.1";
		NSString *currSysVer = [[UIDevice currentDevice] systemVersion];
		if ([currSysVer compare:reqSysVer options:NSNumericSearch] != NSOrderedAscending)
			displayLinkSupported = TRUE;

        // multitouch
        self.multipleTouchEnabled = YES;
        touchPosition.x = 0;
        touchPosition.y= 0;
        scale = 1;
    }
	
    return self;
}

- (void) drawView:(id)sender
{
    aniChrono.stop();
    //std::cout<<"ani:"<<aniChrono.m_lapse*1e-03<<std::endl;
    float aniLapse = (0.9*aniChrono.m_lapse*1e-03);
    float aniInterval = (1.0 / 60.0) * animationFrameInterval;
    
    aniChrono.reset();
    aniChrono.start();

    if (aniLapse <= aniInterval) {
       
        //[EAGLContext setCurrentContext:_context];
        [renderer render];
        
        [renderer runWithLow:lowAcc AndHi:hiAcc AndPos:touchPosition AndDeltaPos:deltaPosition AndDeltaDistance:deltaDistance AndType:touchType AndFingers:touchFingers AndTouchTaps:touchTaps];
        
        touchType = touchType == eTapTouch? eNoTouch:touchType;
        
        deltaPosition.x=0;
        deltaPosition.y=0;

    }
}

- (void) layoutSubviews
{
	[renderer resizeFromLayer:(CAEAGLLayer*)self.layer];
    [self drawView:nil];
}

- (NSInteger) animationFrameInterval
{
	return animationFrameInterval;
}

- (void) setAnimationFrameInterval:(NSInteger)frameInterval
{
	// Frame interval defines how many display frames must pass between each time the
	// display link fires. The display link will only fire 30 times a second when the
	// frame internal is 2 on a display that refreshes 60 times a second. The default
	// frame interval setting of 1 will fire 60 times a second when the display refreshes
	// at 60 times a second. A frame interval setting of less than one results in undefined
	// behavior.
	if (frameInterval >= 1)
	{
		animationFrameInterval = frameInterval;
		
		if (animating)
		{
			[self stopAnimation];
			[self startAnimation];
		}
	}
}

- (void) startAnimation
{
	if (!animating)
	{
		if (displayLinkSupported)
		{
			// CADisplayLink is API new to iPhone SDK 3.1. Compiling against earlier versions will result in a warning, but can be dismissed
			// if the system version runtime check for CADisplayLink exists in -initWithCoder:. The runtime check ensures this code will
			// not be called in system versions earlier than 3.1.
			
			displayLink = [NSClassFromString(@"CADisplayLink") displayLinkWithTarget:self selector:@selector(drawView:)];
			[displayLink setFrameInterval:animationFrameInterval];
			[displayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
		}
		else
			animationTimer = [NSTimer scheduledTimerWithTimeInterval:(NSTimeInterval)((1.0 / 60.0) * animationFrameInterval) target:self selector:@selector(drawView:) userInfo:nil repeats:TRUE];
		
		animating = TRUE;
	}
}

- (void)stopAnimation
{
	if (animating) {
		if (displayLinkSupported) {
			[displayLink invalidate];
			displayLink = nil;
		}
		else {
			[animationTimer invalidate];
			animationTimer = nil;
		}		
		animating = FALSE;
	}
}

- (void) dealloc
{
    [renderer release];
	
	// tear down context
	if ([EAGLContext currentContext] == _context)
        [EAGLContext setCurrentContext:nil];
	
	[_context release];
	
    [super dealloc];
}

// Multitouch



- (CGFloat)distanceBetweenTwoPoints:(CGPoint)fromPoint toPoint:(CGPoint)toPoint {
    float x = toPoint.x - fromPoint.x;
    float y = toPoint.y - fromPoint.y;
    return sqrt(x * x + y * y);
}

- (CGPoint)midPoint:(CGPoint)fromPoint toPoint:(CGPoint)toPoint {
    CGPoint ret;
    ret.x = (toPoint.x + fromPoint.x)/2;
    ret.y = (toPoint.y + fromPoint.y)/2;
    return ret;
}


-(void)handleSingleTap {
    touchTaps = eTapSingle;
    touchType = eTapTouch;
}

// Multi-touch
- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event {

    std::cout<<"touch"<<std::endl;
    NSSet *allTouches = [event allTouches];
    UITouch *touch1 = [[allTouches allObjects] objectAtIndex:0];
    if ([touch1 view] == self) {
        
        
        switch ([allTouches count]) 
        {
            case 1:  //Single touch
            {
                touchFingers = eOneFinger;
                
                initialPosition = touchPosition = [touch1 locationInView:self];
                
                switch ([touch1 tapCount])
                {
                    case 1: //Single Tap.
                    {} 
                        break;
                    case 2: //Double tap.
                    {
                        [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(handleSingleTap) object:nil];

                    } 
                    break;
                }
            } 
                break;
            case 2:  //Double Touch
            {
                touchFingers = eTwoFingers;
                
                //Track the initial distance between two fingers.
                UITouch *touch1 = [[allTouches allObjects] objectAtIndex:0];
                UITouch *touch2 = [[allTouches allObjects] objectAtIndex:1];
                
                initialPosition = [ self midPoint:[touch1 locationInView:self] toPoint:[touch2 locationInView:self] ];
                
                touchDistance = [self distanceBetweenTwoPoints:[touch1 locationInView:self] toPoint:[touch2 locationInView:self] ];
                //initialDistance/= scale;
            }
                break;
            default:
                break;
        }
    }
}

- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event {
	
    NSSet *allTouches = [event allTouches];
    
    UITouch *touch1 = [[allTouches allObjects] objectAtIndex:0];
    
	if ([touch1 view] == self) {
        NSUInteger tapCount = [touch1 tapCount];
        
        if (touchType== eSweepTouch) {
            touchType = eNoTouch;
        }
        else {
            switch (tapCount) {
                case 1:
                    [self performSelector:@selector(handleSingleTap) withObject:nil afterDelay:.4];
                    break;
                case 2:
                    touchTaps = eTapDouble;
                    touchType = eTapTouch;
                    break;
                default:
                    break;
            }
        }
        
	}
    //scale = 1;
    
}



- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event {
    
    NSSet *allTouches = [event allTouches];
    UITouch *touch1 = [[allTouches allObjects] objectAtIndex:0];
	if ([touch1 view] == self) {        
        
//        for(id touch in allTouches) {
//            CGPoint prevLocation = [touch previousLocationInView:self];
//            if (touchPosition.x == prevLocation.x && touchPosition.y == prevLocation.y ) {
//                
//                CGPoint finalPosition = [touch locationInView:self];
//                deltaPosition.x = finalPosition.x-prevLocation.x;
//                deltaPosition.y = finalPosition.y-prevLocation.y;
//            }
//        }
//        
        touchType = eSweepTouch;
        
        switch ([allTouches count])
        {
            case 1: 
            {
                touchFingers = eOneFinger;
                CGPoint prevLocation = [touch1 previousLocationInView:self];
                if (touchPosition.x == prevLocation.x && touchPosition.y == prevLocation.y ) {
                    touchPosition = [touch1 locationInView:self];
                    
                    deltaPosition.x = touchPosition.x-prevLocation.x;
                    deltaPosition.y = touchPosition.y-prevLocation.y;
                }
            } 
                break;
            case 2: 
            {
                touchFingers = eTwoFingers;
                
                UITouch *touch2 = [[allTouches allObjects] objectAtIndex:1];
                
                CGPoint prevPosition = touchPosition;
                touchPosition = [ self midPoint:[touch1 locationInView:self] toPoint:[touch2 locationInView:self] ];
                deltaPosition.x = touchPosition.x-prevPosition.x;
                deltaPosition.y = touchPosition.y-prevPosition.y; 
                
                
                prevDistance = touchDistance;
                touchDistance = [ self distanceBetweenTwoPoints:[touch1 locationInView:self] toPoint:[touch2 locationInView:self] ];
                deltaDistance = touchDistance-prevDistance;
                
            } 
                break;
        }
    }
}




- (void)touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event {
    //UITouch *touch = [touches anyObject];
	/*
     To impose as little impact on the device as possible
     */
    // discard touch swipe
}


- (void)setSlider0Value:(float) value{
    [renderer setSlider0Value:value];
}


@end
