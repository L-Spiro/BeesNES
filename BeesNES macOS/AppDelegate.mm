//
//  AppDelegate.m
//  BeesNES macOS
//
//  Created by Shawn Wilcoxen on 2024/03/28.
//

#include "../Src/LSNLSpiroNes.h"
#include "../Src/Audio/LSNAudio.h"
#include "../Src/Database/LSNDatabase.h"

#import "AppDelegate.h"

@interface AppDelegate ()

@end

@implementation AppDelegate

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
    lsn::CDatabase::Init();
    lsn::CAudio::InitializeAudio();
}


- (void)applicationWillTerminate:(NSNotification *)aNotification {
    lsn::CAudio::ShutdownAudio();
    lsn::CDatabase::Reset();
}


- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)sender {
    return YES;
}


@end
