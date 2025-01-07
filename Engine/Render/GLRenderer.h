//
//  EAGLView.h
//  SampleApp
//
//  Created by artem on 9/5/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//


#ifndef __PYENGINE_2_0_OGL_GPUScreen_H___
#define __PYENGINE_2_0_OGL_GPUScreen_H___

#define NOMINMAX
// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"	
#if APIABSTRACTION_OGL


// Outer-Engine includes

#define GL3_PROTOTYPES 1

#if defined(SN_TARGET_PS3)
#   include <PSGL/psgl.h>
#   include <PSGL/psglu.h>
#endif

#if APIABSTRACTION_GLPC

	#include "PrimeEngine/Application/WinApplication.h"
	// Win32 GL stuff
	// Let VC++ know we are compiling for WinXP and up.  This let's us use
	// API functions specific to WinXP (e.g., WM_INPUT API).
	#ifndef _WIN32_WINNT
		#define _WIN32_WINNT   0x0600 // Vista
	#endif
	#include "windows.h"

	#define GLUT_NO_LIB_PRAGMA
	#include <glew.h>
	#include <wglew.h>

	//# include <wglext.h>
	
	#include "CG/cg.h"
	#include "CG/cggl.h"
#endif // #if APIABSTRACTION_GLPC

#if APIABSTRACTION_PS3
#define peglGenFramebuffers glGenFramebuffersOES
#define peglBindFramebuffer glBindFramebufferOES
#define peglFramebufferTexture2D glFramebufferTexture2DOES
#else
#define peglGenFramebuffers glGenFramebuffers
#define peglBindFramebuffer glBindFramebuffer
#define peglFramebufferTexture2D glFramebufferTexture2D
#endif


#if APIABSTRACTION_IOS
#import <QuartzCore/QuartzCore.h>
#   import <UIKit/UIKit.h>
    #import <CoreFoundation/CFBase.h>

#      import <OpenGLES/EAGL.h>

#   import <OpenGLES/ES1/gl.h>
#   import <OpenGLES/ES1/glext.h>
#   import <OpenGLES/ES2/gl.h>
#   import <OpenGLES/ES2/glext.h>
#   include "glues/source/glues.h"

#endif


// Inter-Engine includes
#include "PrimeEngine/MemoryManagement/Handle.h"
#include "PrimeEngine/Utils/PEClassDecl.h"

#include "PrimeEngine/Math/Vector4.h"
#include "PrimeEngine/Render/IRenderer.h"

#if APIABSTRACTION_IOS
//code for IOS

@class IOSMainRenderView;
@class IOSGameRenderViewController;

#endif // #if APIABSTRACTION_IOS
namespace PE {
    
    // forward declaration of texture class
    struct TextureGPU;
    
    class GLRenderer : public IRenderer
    {
    public:
        
        void initialiseContext();
        void uninitialiseContext();
       
        virtual void setRenderTargetsAndViewportWithDepth(TextureGPU *pDestColorTex = 0, TextureGPU *pDestDepthTex = 0, bool clearRenderTargte = false, bool clearDepth = false);
        
        virtual void setDepthStencilOnlyRenderTargetAndViewport(TextureGPU *pDestDepthTex, bool clear = false);
        
        virtual void setRenderTargetsAndViewportWithNoDepth(TextureGPU *pDestColorTex = 0, bool clear = false);
    
		virtual PrimitiveTypes::UInt32 getWidth(){return m_width;}
		virtual PrimitiveTypes::UInt32 getHeight(){return m_height;}
		virtual void setClearColor(Vector4 color){m_clearColor = color;}
		virtual void setVSync(bool useVsync);
		
		virtual void swap(bool vsync = 0);
		virtual void clear();
		virtual void endRenderTarget(TextureGPU *pTex);
		virtual void endFrame();

		bool checkForErrors(const char *situation);

        static const char* outputGLBoolean(const char *prefix, GLenum glenum, const char* postfix = "\n");
        static const char* outputGLint(const char *prefix, GLenum glenum, const char* postfix = "\n");
        static char s_dbgStr[80];
        static bool s_debugGfx;
    private:
 

        GLRenderer(PE::GameContext &context, unsigned int width, unsigned int height);
        #if APIABSTRACTION_PS3
            void gfxRegisterSysutilCallback();
        #else
        
        #endif
        
    public:
              
        void clearDepthAndStencil()
        {

        }
        
        void processOSEventsIntoGlobalEventQueue()
        {

        }
        
        
        // Accessors ---------------------------------------------------------------

    public:
        
    public:
        #if !APIABSTRACTION_IOS        
            CGcontext m_cgContext;
            CGprofile m_cgVertexProfile;
            CGprofile m_cgFragmentProfile;
        #else
            static IOSGameRenderViewController *s_pViewController;
            EAGLContext *context;
        #endif
		#if APIABSTRACTION_GLPC
			HGLRC m_hglrc;
			HDC m_hdc;
		#endif
		GLuint m_frameBufferObject;

        Vector4 m_clearColor;
       
        PrimitiveTypes::UInt32 m_width, m_height;

		PE::GameContext *m_pContext;
		friend class IRenderer;

    };
}; // namespace PE


#endif // APIABstraction
#endif // File guard
