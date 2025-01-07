//
//  GameIOSInput.cpp
//  SampleWorkspace
//
//  Created by student on 11/6/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#define IOS_INPUT_PRINTS 0
#import "GameIOSInput.h"
#include "PrimeEngine/Game/Client/ClientGame.h"

/*
@implementation IOSGameRenderViewController (UIViewController)


//these aren't getting called currently, the ones in Game.mm are

- (CGPoint) calcMiddleTouch:(NSSet *)touches withEvent:(UIEvent *)event
{
    CGPoint sum;
    sum.x = sum.y = 0;
    
    int i = 0;
    
    for(UITouch *touch in touches)
    {
        //UITouch *touch = [touches objectAtIndex:i];
        CGPoint location = [touch locationInView:touch.view];
        sum.x += location.x;
        sum.y += location.y;
#if IOS_INPUT_PRINTS
        NSLog(@"Touch[%d] Moved at (%f, %f)\n", i, location.x, location.y);
#endif
        i++;
    }
    if (touches.count)
    {
        sum.x /= touches.count;
        sum.y /= touches.count;
    }
    
    CGPoint location = sum;
#if IOS_INPUT_PRINTS
    NSLog(@"Touch Middle Moved at (%f, %f)\n", location.x, location.y);
#endif
    return location;
}

- (void) touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event {
    
    UITouch *touch = [[event allTouches] anyObject];
    
    CGPoint location = [touch locationInView:touch.view];

    numTouches += touches.count;
#if IOS_INPUT_PRINTS
    NSLog(@"Touches Began (%d) now NumTouces: %d\n", touches.count, numTouches);
#endif
    if (numTouches == 1)
    {
        CGPoint midlocation = [self calcMiddleTouch:touches withEvent:event];
        lastPosition = midlocation;
        m_lastUpdatedNumTouches = 1; // in case it is the first touch, we dont need to skip. this = 1 will allow to generate mvoe event on next touchesMoved callback
    }
    else
        m_lastUpdatedNumTouches = -1; // make sure we reset the system
}

- (void) touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event {
   
    //UITouch *touch = [[event allTouches] anyObject];
    
    UITouch *touch = [touches anyObject];
    CGPoint location = [touch locationInView:touch.view];
    
    if (touches.count != numTouches)
    {
        // this happens when additional touches are added while some touches exist
        // a simple way to handle with this is to skip this case
        //  since next event sent will have all touches together
        return;
    }
    
    PE::Handle hTouchMovedEvent("EVENT", sizeof(PE::Events::Event_IOS_TOUCH_MOVED));
    PE::Events::Event_IOS_TOUCH_MOVED *pTouchMovedEvent = new(hTouchMovedEvent) PE::Events::Event_IOS_TOUCH_MOVED ;
    
    location = [self calcMiddleTouch:touches withEvent:event];
    
    //check if number of fingers changed so that we dont accidentally generate bad movement event
    if (m_lastUpdatedNumTouches != numTouches)
    {
        lastPosition = location;
        m_lastUpdatedNumTouches = numTouches;
        return; 
    }
    
    pTouchMovedEvent->m_dx = location.x - lastPosition.x;
    pTouchMovedEvent->m_dy = location.y - lastPosition.y;
    
    
    pTouchMovedEvent->m_normalized_dx = 2 * pTouchMovedEvent->m_dx / (PrimitiveTypes::Float32)(PE::GPUScreen::Instance()->getWidth());
    pTouchMovedEvent->m_normalized_dy = 2 * pTouchMovedEvent->m_dy / (PrimitiveTypes::Float32)(PE::GPUScreen::Instance()->getHeight());
    
    pTouchMovedEvent->touches = touches;
    
    
    pTouchMovedEvent->touchesCount = touches.count;
    
    PE::Events::EventQueueManager::Instance()->add(hTouchMovedEvent, PE::Events::QT_INPUT);
  
    lastPosition = location;
    m_lastUpdatedNumTouches = numTouches;
}

- (void) touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event {
    // pass it to ui
    UITouch *touch = [touches anyObject];
    CGPoint location = [touch locationInView:touch.view];

    numTouches -= touches.count;
#if IOS_INPUT_PRINTS
    NSLog(@"Touches Ended (%d) now numTouces: %d\n", touches.count, numTouches);
#endif
    m_lastUpdatedNumTouches = -1; // make sure we reset the system on next touchesMoved event
    
}

@end

*/