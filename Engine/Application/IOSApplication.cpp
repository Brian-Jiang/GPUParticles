
#define NOMINMAX

// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

// Outer-Engine includes

// Inter-Engine includes

// Sibling/Children includes
#include "IOSApplication.h"
#include "PrimeEngine/Game/Client/ClientGame.h"
#include "PrimeEngine/Game/Client/IOSGameRenderViewController.h"

#import <OpenGLES/ES2/glext.h>
#import <UIKit/UIKit.h>
#import <GLKit/GLKit.h>

namespace PE {

IOSApplication::IOSApplication(PE::GameContext &context, unsigned int width, unsigned int height, const char *caption)
{
}
	
void IOSApplication::exit()
{
}

Application* Application::Construct(PE::GameContext &context, unsigned int width, unsigned int height, const char *caption)
{
	Handle h("IOSApplication", sizeof(IOSApplication));
	IOSApplication *pApp = new (h) IOSApplication(context, width, height, caption);
	context.m_pApplication = pApp;
	return pApp;
}

}; // namespace PE


@implementation IOSAppDelegate

@synthesize window;
@synthesize viewController;

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{
	// the Engine and Game have been initialised by now.
	// starting to catch os events
	
	// we could potentially do more initialization here..
	
    // application - is the object started from main with this object as event delegate
    
    // first we need window
    
	printf("PE: Progress: AppDelegate::didFinishLaunchingWithOptions Entry..\n");
    
    UIScreen *screen = [UIScreen mainScreen]; // get ptr to main screen
    CGRect bounds = [screen bounds];
    
    PEINFO("mainScreen bounds are x: %.2f y: %.2f w: %.2f h: %.2f\n", bounds.origin.x, bounds.origin.y, bounds.size.width, bounds.size.height);
    
    self.window = [[[UIWindow alloc] initWithFrame:[[UIScreen mainScreen] bounds]] autorelease];
    
    
    IOSGameRenderViewController *baseViewController = [[IOSGameRenderViewController alloc] init];
    self.viewController = baseViewController;
    
    self.viewController.m_pGameContext = &PE::Components::ClientGame::s_context;
    
    // this call will perform the rest of game initialization.
    // it will also set view controller's view
    //
    //e.g. this code: self.viewController.view = baseView; where baseView is created in GLRenderer
    
    [self.viewController awakeFromNib];
    
    GLKView *createdView = (GLKView *)(self.viewController.view);
    PEASSERT(createdView != NULL, "View was not created");
    
    createdView.delegate = self.viewController;
    
    // [window addSubview:baseView]; // this will call viewWillAppear on the controller
    
    // this call sets up root view of window by setting root view controller, that
    // has the view already setup
    self.window.rootViewController = self.viewController; // this property assignment ends up calling viewController.viewDidLoad
    
    [self.viewController viewDidLoad];
    
    
    [self.window makeKeyAndVisible];
    
    return YES;
}

- (void)applicationWillResignActive:(UIApplication *)application
{
	// pause game (home button was pressed)
    [self.viewController pauseGame];
}

- (void)applicationDidBecomeActive:(UIApplication *)application
{
	// the application was either just created or returned from paused
	[self.viewController unpauseGame];
}

- (void)applicationWillTerminate:(UIApplication *)application
{
    [self.viewController pauseGame];
}

- (void)applicationDidEnterBackground:(UIApplication *)application
{
    // Handle any background procedures not related to animation here.
}

- (void)applicationWillEnterForeground:(UIApplication *)application
{
    // Handle any foreground procedures not related to animation here.
}

- (void)dealloc
{
    [viewController release];
    [window release];
    
    [super dealloc];
}

@end