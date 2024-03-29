//
//  AppDelegate.m
//  BeesNES macOS
//
//  Created by Shawn Wilcoxen on 2024/03/28.
//

#include "../Src/LSNLSpiroNes.h"
#include "../Src/Audio/LSNAudio.h"

#import "AppDelegate.h"

@interface AppDelegate ()

@end

@implementation AppDelegate

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
    // Insert code here to initialize your application
}


- (void)applicationWillTerminate:(NSNotification *)aNotification {
    // Insert code here to tear down your application
}


- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)sender {
    return YES;
}


@end
