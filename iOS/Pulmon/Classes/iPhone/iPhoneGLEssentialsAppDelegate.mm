/*
     File: iPhoneGLEssentialPracticesAppDelegate.m
 Abstract: The application delegate.
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

#import "iPhoneGLEssentialsAppDelegate.h"
#import "EAGLView.h"

#define kAccelerometerFrequency		100.0 // Hz
#define kLowFilterFactor			0.1
#define kHiFilterFactor             0.9

@implementation iPhoneGLEssentialPracticesAppDelegate

@synthesize window;
@synthesize glView;
@synthesize slider0;

#pragma mark -
#pragma mark Application lifecycle

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{    
    
    // Override point for customization after application launch.
	
	[glView startAnimation];
    [window makeKeyAndVisible];
    
   	//Configure and start accelerometer
	[[UIAccelerometer sharedAccelerometer] setUpdateInterval:(1.0 / kAccelerometerFrequency)];
	[[UIAccelerometer sharedAccelerometer] setDelegate:self];
    
    return YES;
}


- (void)applicationWillResignActive:(UIApplication *)application
{
	/*
     Sent when the application is about to move from active to inactive state. This can occur for certain types of temporary interruptions (such as an incoming phone call or SMS message) or when the user quits the application and it begins the transition to the background state.
     Use this method to pause ongoing tasks, disable timers, and throttle down OpenGL ES frame rates. Games should use this method to pause the game.
     */

	[glView stopAnimation];
}


- (void)applicationDidEnterBackground:(UIApplication *)application {
    /*
     Use this method to release shared resources, save user data, invalidate timers, and store enough application state information to restore your application to its current state in case it is terminated later. 
     If your application supports background execution, called instead of applicationWillTerminate: when the user quits.
     */
	
	[glView stopAnimation];
    //[glView dealloc];
}


- (void)applicationWillEnterForeground:(UIApplication *)application {
    /*
     Called as part of transition from the background to the inactive state: here you can undo many of the changes made on entering the background.
     */
	
	[glView startAnimation];
}


- (void)applicationDidBecomeActive:(UIApplication *)application 
{
    /*
     Restart any tasks that were paused (or not yet started) while the application was inactive. If the application was previously in the background, optionally refresh the user interface.
     */
	
	[glView startAnimation];
}


- (void)applicationWillTerminate:(UIApplication *)application {
    /*
     Called when the application is about to terminate.
     See also applicationDidEnterBackground:.
     */
	
	[glView stopAnimation];

}


// Accelerometer
- (void)accelerometer:(UIAccelerometer*)accelerometer didAccelerate:(UIAcceleration*)acceleration
{
	//Use a basic low-pass filter to only keep the gravity in the accelerometer values
    Point3 low;
    low.x = acceleration.x * kLowFilterFactor + glView.lowAcc.x * (1.0 - kLowFilterFactor);
    low.y = acceleration.y * kLowFilterFactor + glView.lowAcc.y * (1.0 - kLowFilterFactor);
    low.z = acceleration.z * kLowFilterFactor + glView.lowAcc.z * (1.0 - kLowFilterFactor);
    glView.lowAcc = low;

//	glView.lowAccelX = acceleration.x * kLowFilterFactor + glView.lowAccelX * (1.0 - kLowFilterFactor);
//	glView.lowAccelY = acceleration.y * kLowFilterFactor + glView.lowAccelY * (1.0 - kLowFilterFactor);
//	glView.lowAccelZ = acceleration.z * kLowFilterFactor + glView.lowAccelZ * (1.0 - kLowFilterFactor);

    Point3 hi;
	hi.x = acceleration.x * kHiFilterFactor + glView.hiAcc.x * (1.0 - kHiFilterFactor);
	hi.y = acceleration.y * kHiFilterFactor + glView.hiAcc.y * (1.0 - kHiFilterFactor);
	hi.z = acceleration.z * kHiFilterFactor + glView.hiAcc.z * (1.0 - kHiFilterFactor);

    glView.hiAcc = hi;
//    glView.hiAccelX = acceleration.x * kHiFilterFactor + glView.hiAccelX * (1.0 - kHiFilterFactor);
//	glView.hiAccelY = acceleration.y * kHiFilterFactor + glView.hiAccelY * (1.0 - kHiFilterFactor);
//	glView.hiAccelZ = acceleration.z * kHiFilterFactor + glView.hiAccelZ * (1.0 - kHiFilterFactor);

}


#pragma mark -
#pragma mark Memory management

- (void)applicationDidReceiveMemoryWarning:(UIApplication *)application {
    /*
     Free up as much memory as possible by purging cached data objects that can be recreated (or reloaded from disk) later.
     */
}


- (void)dealloc {
    [glView release];
    [window release];
    [slider0 release];
    [super dealloc];
}


- (IBAction)handleSlider0ValueChanged:(id)sender {
    UISlider *slider = (UISlider *)sender;
    [glView setSlider0Value:[slider value]]; 
}
@end
