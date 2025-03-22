/*
Author: Ryan Wiseman

Macs have a very specific menu call approach that isn't accessible by
SDL. Because of it, we are required to use Obj-C, particularly, 
the Cocoa library.
*/

#ifdef __APPLE__
#import <Cocoa/Cocoa.h>
#import "cocoaToolbarHandler.h"

// Simplified AppDelegate declaration
@interface AppDelegate : NSObject <NSApplicationDelegate>
- (IBAction)customAboutAction:(id)sender;
@end

@implementation AppDelegate

- (IBAction)customAboutAction:(id)sender {
    NSLog(@"customAboutAction triggered");
    openSDLWindowAboutMenu();
}

@end

static AppDelegate *sharedDelegate = nil;
static NSWindow *aboutWindow = nil;

extern "C" void openSDLWindowAboutMenu() {
    if (aboutWindow != nil) {
        [aboutWindow makeKeyAndOrderFront:nil];
        return;
    }

    NSRect frame = NSMakeRect(0, 0, 500, 400);
    aboutWindow = [[NSWindow alloc] initWithContentRect:frame
                                              styleMask:(NSWindowStyleMaskTitled | NSWindowStyleMaskClosable)
                                                backing:NSBackingStoreBuffered
                                                  defer:NO];
    [aboutWindow center];
    [aboutWindow setTitle:@"About Cat Tac Toe"];

    NSTextView *textView = [[NSTextView alloc] initWithFrame:NSMakeRect(0, 0, 500, 400)];
    
    // Create a multi-line string with each test string on its own line
    NSString *aboutText = @"Cat Tac Toe Build 0.1.\n"
                          @"Welcome to this game!\n\n"
                          @"Hi, my name is Ryan Wiseman, I am the developer behind this simple game\n"
                          @"This was constructed with SDL3 and FFMPEG! A feat that I am very proud of\n\n"
                          @"One of the ways you can support me is to stream my own music!\n"
                          @"Outside of developing software, I also write a good amount of music\n"
                          @"As I continue to progress through various small projects, I hope to someday\n"
                          @"develop audio plugins that are ones you can pay for and use\n";
    
    [textView setString:aboutText];
    [textView setEditable:NO];
    [textView setSelectable:YES];

    NSScrollView *scrollView = [[NSScrollView alloc] initWithFrame:frame];
    [scrollView setDocumentView:textView];
    [scrollView setHasVerticalScroller:YES];

    [aboutWindow setContentView:scrollView];
    [aboutWindow makeKeyAndOrderFront:nil];
}

extern "C" void cocoaBaseMenuBar() {
    NSApplication *app = [NSApplication sharedApplication];
    
    sharedDelegate = [[AppDelegate alloc] init];
    [app setDelegate:sharedDelegate];
    
    NSMenu *menuBar = [[NSMenu alloc] init];
    NSMenuItem *appMenuItem = [[NSMenuItem alloc] init];
    [menuBar addItem:appMenuItem];
    
    // Use a constant app name instead of trying to modify the process name
    NSString *appName = @"Cat Tac Toe";
    
    NSMenu *appMenu = [[NSMenu alloc] init];
    
    NSMenuItem *aboutMenuItem = [[NSMenuItem alloc] initWithTitle:[NSString stringWithFormat:@"About %@", appName]
                                                          action:@selector(customAboutAction:)
                                                   keyEquivalent:@""];
    [aboutMenuItem setTarget:sharedDelegate];
    
    NSMenuItem *quitMenuItem = [[NSMenuItem alloc] initWithTitle:@"Quit"
                                                         action:@selector(terminate:)
                                                  keyEquivalent:@"q"];
    
    [appMenu addItem:aboutMenuItem];
    [appMenu addItem:quitMenuItem];
    [appMenuItem setSubmenu:appMenu];
    
    [app setMainMenu:menuBar];
}
#else

extern "C" void openSDLWindowAboutMenu() {
}

extern "C" void cocoaBaseMenuBar() {
}
#endif
