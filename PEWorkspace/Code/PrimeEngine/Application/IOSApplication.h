#ifndef __pe_iosapplication_h__
#define __pe_iosapplication_h__

// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

#import <QuartzCore/QuartzCore.h>
#import <UIKit/UIKit.h>
#import <CoreFoundation/CFBase.h>

// Inter-Engine includes
#include "PrimeEngine/MainFunction/MainFunctionArgs.h"
#include "PrimeEngine/MemoryManagement/Handle.h"
#include "PrimeEngine/Events/EventQueueManager.h"
#include "PrimeEngine/Events/StandardEvents.h"
#include "Application.h"

// Sibling/Children includes

// This class is implementation of IRenderer for D3D10

namespace PE {

// forward declaration of texture class
struct TextureGPU;

class IOSApplication : public Application
{
public:
	IOSApplication(PE::GameContext &context, unsigned int width, unsigned int height, const char *caption);
		
	virtual void processOSEventsIntoGlobalEventQueue()
	{
	}

	virtual void exit();

public:
	
};
}; // namespace PE

@class IOSGameRenderViewController;
    
@interface IOSAppDelegate : NSObject <UIApplicationDelegate> {
    UIWindow *window;
    IOSGameRenderViewController *viewController;
}

@property (nonatomic, retain) IBOutlet UIWindow *window;
@property (nonatomic, retain) IBOutlet IOSGameRenderViewController *viewController;

@end


#endif // File guard
