

#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

#include "IOSGameRenderViewController.h"

#include "PrimeEngine/PrimeEngineIncludes.h"
#include "PrimeEngine/RenderJob.h"
#include "PrimeEngine/GameThreadJob.h"
#include "PrimeEngine/Application/Application.h"

#if APIABSTRACTION_IOS
#import <QuartzCore/QuartzCore.h>
#import <UIKit/UIKit.h>
#import <CoreData/CoreData.h>
#import "GameIOSInput.h"

#import <OpenGLES/ES2/glext.h>

#import <UIKit/UIKit.h>
#import <GLKit/GLKit.h>

#endif

// Attribute index.
enum {
    ATTRIB_VERTEX,
    ATTRIB_COLOR,
    NUM_ATTRIBUTES
};

@interface IOSGameRenderViewController()
@property (nonatomic, retain) EAGLContext *context;

- (BOOL)loadShaders;
- (BOOL)compileShader:(GLuint *)shader type:(GLenum)type file:(NSString *)file;
- (BOOL)linkProgram:(GLuint)prog;
- (BOOL)validateProgram:(GLuint)prog;



@end

@implementation IOSGameRenderViewController

@synthesize animating, context;

@synthesize lastPosition, m_lastUpdatedNumTouches, numTouches, m_pGameContext;

- (void)awakeFromNib
{
	PEINFO("Game::awakeFromNib Entry..\n");
    
    PE::GLRenderer::s_pViewController = self;
    
    //#define RETURN_VALUE // also need this to compile
    #include "MainFunctionEnd.h"

    
    //if ([context API] == kEAGLRenderingAPIOpenGLES2)
        [self loadShaders];
    
    animating = FALSE;
    animationFrameInterval = 1;
    
    [self.view setMultipleTouchEnabled:YES];
    numTouches = 0;
    m_lastUpdatedNumTouches = -1;
}

- (void)dealloc
{
    // Tear down context.
    if ([EAGLContext currentContext] == context)
        [EAGLContext setCurrentContext:nil];
    
    [context release];
    
    [super dealloc];
}

- (void)viewWillAppear:(BOOL)animated
{
    [self unpauseGame];
    
    [super viewWillAppear:animated];
}

- (void)viewWillDisappear:(BOOL)animated
{
    [self pauseGame];
    
    [super viewWillDisappear:animated];
}

- (void)viewDidUnload
{
	[super viewDidUnload];

    printf("Unintialise Context Here: FIX IT!\n");	
}


- (void)viewDidLoad
{
	[super viewDidLoad];
    
    
    // [self setupGL];
    // gl renderer is created after this
    
    //[view setNeedsDisplay];
    
    printf("viewDidLoad\n");
}


- (void)update
{
#if 0
    float aspect = fabsf(self.view.bounds.size.width / self.view.bounds.size.height);
    GLKMatrix4 projectionMatrix = GLKMatrix4MakePerspective(GLKMathDegreesToRadians(65.0f), aspect, 0.1f, 100.0f);
    
    self.effect.transform.projectionMatrix = projectionMatrix;
    
    GLKMatrix4 baseModelViewMatrix = GLKMatrix4MakeTranslation(0.0f, 0.0f, -4.0f);
    baseModelViewMatrix = GLKMatrix4Rotate(baseModelViewMatrix, _rotation, 0.0f, 1.0f, 0.0f);
    
    // Compute the model view matrix for the object rendered with GLKit
    GLKMatrix4 modelViewMatrix = GLKMatrix4MakeTranslation(0.0f, 0.0f, -1.5f);
    modelViewMatrix = GLKMatrix4Rotate(modelViewMatrix, _rotation, 1.0f, 1.0f, 1.0f);
    modelViewMatrix = GLKMatrix4Multiply(baseModelViewMatrix, modelViewMatrix);
    
    self.effect.transform.modelviewMatrix = modelViewMatrix;
    
    // Compute the model view matrix for the object rendered with ES2
    modelViewMatrix = GLKMatrix4MakeTranslation(0.0f, 0.0f, 1.5f);
    modelViewMatrix = GLKMatrix4Rotate(modelViewMatrix, _rotation, 1.0f, 1.0f, 1.0f);
    modelViewMatrix = GLKMatrix4Multiply(baseModelViewMatrix, modelViewMatrix);
    
    _normalMatrix = GLKMatrix3InvertAndTranspose(GLKMatrix4GetMatrix3(modelViewMatrix), NULL);
    
    _modelViewProjectionMatrix = GLKMatrix4Multiply(projectionMatrix, modelViewMatrix);
    
    _rotation += self.timeSinceLastUpdate * 0.5f;
#endif
    
    //((GLKView*)(self.view)).enableSetNeedsDisplay = true;
    
    //[self.view setNeedsDisplay];

    //CGRect rect;
    
    //[self.view setNeedsDisplayInRect: CGRectMake(0, 0, 100, 100)] ;
    
    
    //PEINFO("enableSetNeedsDisplay: %d\n", int(((GLKView*)(self.view)).enableSetNeedsDisplay));
    
    PEINFO("view update\n");
    
    
}

- (void)glkView:(GLKView *)view drawInRect:(CGRect)rect
{
    PE::IRenderer::checkForErrors("");

    [self runSingleFrame];
    
    PE::IRenderer::checkForErrors("");
}


- (NSInteger)animationFrameInterval
{
    return animationFrameInterval;
}

- (void)setAnimationFrameInterval:(NSInteger)frameInterval
{
    /*
	 Frame interval defines how many display frames must pass between each time the display link fires.
	 The display link will only fire 30 times a second when the frame internal is two on a display that refreshes 60 times a second. The default frame interval setting of one will fire 60 times a second when the display refreshes at 60 times a second. A frame interval setting of less than one results in undefined behavior.
	 */
    if (frameInterval >= 1)
    {
        animationFrameInterval = frameInterval;
        
        if (animating)
        {
            [self pauseGame];
            [self unpauseGame];
        }
    }
}

- (void)unpauseGame
{
	printf("PE: Progress: Game::unpauseGame Entry..\n");
}

- (void)pauseGame
{
	printf("PE: Progress: Game::pauseGame Entry..\n");

}

- (void)didReceiveMemoryWarning
{
    // Releases the view if it doesn't have a superview.
    [super didReceiveMemoryWarning];
    
    // Release any cached data, images, etc. that aren't in use.
}


- (void)runSingleFrame
{
	//printf("IOSGameRenderViewController::runSingleFrame[%d] Entry..\n", m_frameIndex);
    PE::Components::ClientGlobalGameCallbacks::getGameInstance()->runGameFrame();
    
	m_frameIndex++;
}

// game input


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
    
    pTouchMovedEvent->m_normalized_dx = 2 * pTouchMovedEvent->m_dx / (PrimitiveTypes::Float32)(m_pGameContext->getGPUScreen()->getWidth());
    pTouchMovedEvent->m_normalized_dy = 2 * pTouchMovedEvent->m_dy / (PrimitiveTypes::Float32)(m_pGameContext->getGPUScreen()->getHeight());
     
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

// Test code for shader loading


- (BOOL)compileShader:(GLuint *)shader type:(GLenum)type file:(NSString *)file
{
    GLint status;
    const GLchar *source;
    
    source = (GLchar *)[[NSString stringWithContentsOfFile:file encoding:NSUTF8StringEncoding error:nil] UTF8String];
    if (!source)
    {
        NSLog(@"Failed to load vertex shader");
        return FALSE;
    }
    
    *shader = glCreateShader(type);
    glShaderSource(*shader, 1, &source, NULL);
    glCompileShader(*shader);
    
#if defined(DEBUG)
    GLint logLength;
    glGetShaderiv(*shader, GL_INFO_LOG_LENGTH, &logLength);
    if (logLength > 0)
    {
        GLchar *log = (GLchar *)malloc(logLength);
        glGetShaderInfoLog(*shader, logLength, &logLength, log);
        NSLog(@"Shader compile log:\n%s", log);
        free(log);
    }
#endif
    
    glGetShaderiv(*shader, GL_COMPILE_STATUS, &status);
    if (status == 0)
    {
        glDeleteShader(*shader);
        return FALSE;
    }
    
    return TRUE;
}

- (BOOL)linkProgram:(GLuint)prog
{
    GLint status;
    
    glLinkProgram(prog);
    
#if defined(DEBUG)
    GLint logLength;
    glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &logLength);
    if (logLength > 0)
    {
        GLchar *log = (GLchar *)malloc(logLength);
        glGetProgramInfoLog(prog, logLength, &logLength, log);
        NSLog(@"Program link log:\n%s", log);
        free(log);
    }
#endif
    
    glGetProgramiv(prog, GL_LINK_STATUS, &status);
    if (status == 0)
        return FALSE;
    
    return TRUE;
}

- (BOOL)validateProgram:(GLuint)prog
{
    GLint logLength, status;
    
    glValidateProgram(prog);
    glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &logLength);
    if (logLength > 0)
    {
        GLchar *log = (GLchar *)malloc(logLength);
        glGetProgramInfoLog(prog, logLength, &logLength, log);
        NSLog(@"Program validate log:\n%s", log);
        free(log);
    }
    
    glGetProgramiv(prog, GL_VALIDATE_STATUS, &status);
    if (status == 0)
        return FALSE;
    
    return TRUE;
}


- (BOOL)loadShaders
{
    GLuint vertShader, fragShader;
    NSString *vertShaderPathname, *fragShaderPathname;
    
    // Create and compile vertex shader.
    vertShaderPathname = [[NSBundle mainBundle] pathForResource:@"Shader" ofType:@"vsh"];
    
    if (![self compileShader:&vertShader type:GL_VERTEX_SHADER file:vertShaderPathname])
    {
        NSLog(@"Failed to compile vertex shader");
        return FALSE;
    }
    
    // Create and compile fragment shader.
    fragShaderPathname = [[NSBundle mainBundle] pathForResource:@"Shader" ofType:@"fsh"];
    if (![self compileShader:&fragShader type:GL_FRAGMENT_SHADER file:fragShaderPathname])
    {
        NSLog(@"Failed to compile fragment shader");
        return FALSE;
    }
    
    // Release vertex and fragment shaders.
    if (vertShader)
        glDeleteShader(vertShader);
    if (fragShader)
        glDeleteShader(fragShader);
    
    return TRUE;
}

@end

