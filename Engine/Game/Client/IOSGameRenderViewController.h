#ifndef __iosgamerenderviewcontroller_h__
#define __iosgamerenderviewcontroller_h__

#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

#include "PrimeEngine/Render/IRenderer.h"
#include "PrimeEngine/Game/Common/GameContext.h"

#import <UIKit/UIKit.h>
#import <OpenGLES/EAGL.h>
#import <OpenGLES/ES1/gl.h>
#import <OpenGLES/ES1/glext.h>
#import <OpenGLES/ES2/gl.h>
#import <OpenGLES/ES2/glext.h>
#import <GLKit/GLKit.h>


@interface IOSGameRenderViewController : GLKViewController
{
    BOOL animating;
    NSInteger animationFrameInterval;
	int m_frameIndex;
    
   CGPoint lastPosition;
   int m_lastUpdatedNumTouches;
   int numTouches;
    
    PE::GameContext *m_pGameContext;
}

@property (readonly, nonatomic, getter=isAnimating) BOOL animating;
@property (nonatomic) NSInteger animationFrameInterval;


@property (nonatomic, assign) CGPoint lastPosition;
@property (nonatomic, assign) int m_lastUpdatedNumTouches;
@property (nonatomic, assign) int numTouches;

@property (nonatomic, assign) PE::GameContext *m_pGameContext;

// game control

- (void)runSingleFrame;

- (void)unpauseGame;
- (void)pauseGame;

// input

- (void) touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event;
- (void) touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event;
- (void) touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event;
- (CGPoint) calcMiddleTouch:(NSSet *)touches withEvent:(UIEvent *)event;


@end

#endif
