//
//  
//  SampleApp
//
//  Created by artem on 9/5/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

#include "GLRenderer.h"

#include "PrimeEngine/Events/EventQueueManager.h"
#include "PrimeEngine/Events/StandardEvents.h"
#include "PrimeEngine/MainFunction/MainFunctionArgs.h"
#include "PrimeEngine/Game/Client/ClientGame.h"
#include "PrimeEngine/Utils/ErrorHandling.h"
#include "PrimeEngine/APIAbstraction/Texture/Texture.h"

// Sibling/Children includes



#if APIABSTRACTION_PS3
#include <cell/dbgfont.h>
#include <cell/sysmodule.h>
#endif

#if APIABSTRACTION_IOS
#import <QuartzCore/QuartzCore.h>

#import <UIKit/UIKit.h>
#import <GLKit/GLKit.h>

#include "PrimeEngine/Game/Client/IOSGameRenderViewController.h"

#endif //if APIABSTRACTION_IOS


namespace PE {

#if APIABSTRACTION_IOS
    IOSGameRenderViewController *GLRenderer::s_pViewController = NULL;
#endif
    
    char GLRenderer::s_dbgStr[80];
    bool GLRenderer::s_debugGfx = true;
    
    void GLRenderer::setRenderTargetsAndViewportWithDepth(TextureGPU *pDestColorTex, TextureGPU *pDestDepthTex, bool clearRenderTarget, bool clearDepth)
    {
		// in DX9 hardware the depth texture is part of the same surface
		// so we need to make sure we are using the same texture
		assert(pDestColorTex == pDestDepthTex);

        static float xadd = 0;
#if 0
        xadd += 0.01f;
        if (xadd > 1.0f)
            xadd = 0;
#endif
        if (pDestColorTex != 0)
		{
           #if APIABSTRACTION_PS3
				glBindFramebufferOES(GL_FRAMEBUFFER_OES, pDestColorTex->m_frameBufferObject); //set framebuffer for reading and writing. could also use GL_READ_FRAMEBUFFER to set a buffer for reading vs writing.
			#else
				glBindFramebuffer(GL_FRAMEBUFFER, pDestColorTex->m_frameBufferObject); //set framebuffer for reading and writing. could also use GL_READ_FRAMEBUFFER to set a buffer for reading vs writing.
			#endif
			glViewport(pDestColorTex->m_viewport.x, pDestColorTex->m_viewport.y, pDestColorTex->m_viewport.w, pDestColorTex->m_viewport.h);
		}
		else
		{
			// use default render target only for now
            #if APIABSTRACTION_IOS

            // need to call bindDrawable here
            
            #else
	
			#if APIABSTRACTION_PS3
				glBindFramebufferOES(GL_FRAMEBUFFER_OES, 0);
			#else
				glBindFramebuffer(GL_FRAMEBUFFER, 0);
			#endif

			glViewport(0, 0, m_width, m_height);
#endif
		}
        IRenderer::checkForErrors("setRenderTargetsAndViewportWithDepth");
        
		IRenderer::checkRenderBufferComplete();

        static float addDefault = 1.0;
        static float addCustom = 0;
        if (clearRenderTarget)
        {
			glClearColor(m_clearColor.m_x + xadd * (pDestColorTex ? 0 : addDefault), m_clearColor.m_y + xadd * (pDestColorTex ? addCustom : 0), m_clearColor.m_z, m_clearColor.m_w);
            if (clearDepth) // color and depth
            {
                float clearDepth = 1.0f;
#if APIABSTRACTION_PS3 || APIABSTRACTION_IOS
                glClearDepthf(clearDepth);
#else
                glClearDepth(clearDepth);
#endif
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            }
            else
            {
                glClear(GL_COLOR_BUFFER_BIT);//color only
            }
        }
        else
        {
            // depth only
            float clearDepth = 1.0f;
#if APIABSTRACTION_PS3 || APIABSTRACTION_IOS
            glClearDepthf(clearDepth);
#else
            glClearDepth(clearDepth);
#endif
            glClear(GL_DEPTH_BUFFER_BIT);
        }
        
    }
    
    void GLRenderer::endRenderTarget(TextureGPU *pTex)
    {
		glFlush();
		#if APIABSTRACTION_PS3
			glBindFramebufferOES(GL_FRAMEBUFFER_OES, 0);
		#else
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		#endif
    }
    
    void GLRenderer::setRenderTargetsAndViewportWithNoDepth(TextureGPU *pDestColorTex, bool clear)
    {
		static float xadd = 0;
		#if 1
			xadd += 0.01f;
			if (xadd > 1.0f)
				xadd = 0;
		#endif
		if (pDestColorTex != 0)
		{
			#if APIABSTRACTION_PS3
				glBindFramebufferOES(GL_FRAMEBUFFER_OES, pDestColorTex->m_frameBufferObject); //set framebuffer for reading and writing. could also use GL_READ_FRAMEBUFFER to set a buffer for reading vs writing.
			#else
				glBindFramebuffer(GL_FRAMEBUFFER, pDestColorTex->m_frameBufferObject); //set framebuffer for reading and writing. could also use GL_READ_FRAMEBUFFER to set a buffer for reading vs writing.
			#endif
			glViewport(pDestColorTex->m_viewport.x, pDestColorTex->m_viewport.y, pDestColorTex->m_viewport.w, pDestColorTex->m_viewport.h);
		}
		else
		{
			// use default render target only for now
#if APIABSTRACTION_IOS
            // need to call bindDrawable here..
#else
			#if APIABSTRACTION_PS3
				glBindFramebufferOES(GL_FRAMEBUFFER_OES, 0);
			#else
				glBindFramebuffer(GL_FRAMEBUFFER, 0);
			#endif
			glViewport(0, 0, m_width, m_height);
#endif
		}
        IRenderer::checkForErrors("setRenderTargetsAndViewportWithNoDepth");
		IRenderer::checkRenderBufferComplete();

		static float add = 0;

		if (clear)
		{
			glClearColor(m_clearColor.m_x + xadd * add, m_clearColor.m_y, m_clearColor.m_z, m_clearColor.m_w);
			glClear(GL_COLOR_BUFFER_BIT);//color only
		}
    }
    
    void GLRenderer::setDepthStencilOnlyRenderTargetAndViewport(TextureGPU *pDestDepthTex, bool clear)
    {
		// the render-to-depth texture is just a color texture
		// it also has the zbuffer attached

		if (pDestDepthTex != 0)
		{

			#if APIABSTRACTION_PS3
				glBindFramebufferOES(GL_FRAMEBUFFER_OES, pDestDepthTex->m_frameBufferObject); //set framebuffer for reading and writing. could also use GL_READ_FRAMEBUFFER to set a buffer for reading vs writing. 
			#else
				glBindFramebuffer(GL_FRAMEBUFFER, pDestDepthTex->m_frameBufferObject); //set framebuffer for reading and writing. could also use GL_READ_FRAMEBUFFER to set a buffer for reading vs writing. 
			#endif
			glViewport(pDestDepthTex->m_viewport.x, pDestDepthTex->m_viewport.y, pDestDepthTex->m_viewport.w, pDestDepthTex->m_viewport.h);	
		}
		else
		{
			// use the default device
			assert(!"Cant render to default device as depth only render");
		}

        IRenderer::checkForErrors("setRenderTargetsAndViewportWithNoDepth");
        IRenderer::checkRenderBufferComplete();
		
		if (clear)
		{
			glClearColor(0, 1.0f, 0, 1.0f);
			float clearDepth = 1.0f;
			#if APIABSTRACTION_PS3 || APIABSTRACTION_IOS
				glClearDepthf(clearDepth);
			#else
				glClearDepth(clearDepth);
			#endif
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		}
    }
    
#if APIABSTRACTION_PS3
    
    static void gfxExitGraphics()
    {
        glFinish();
        exit(0);
    }
    static void gfxSysutilCallback(uint64_t status, uint64_t param, void* userdata)
    {
        (void) param;
        (void) userdata;
        
        switch(status) {
            case CELL_SYSUTIL_REQUEST_EXITGAME:
                gfxExitGraphics();
                break;
            case CELL_SYSUTIL_DRAWING_BEGIN:
            case CELL_SYSUTIL_DRAWING_END:
                break;
            default:
                printf( "Graphics common: Unknown status received: 0x%llx\n", status );
        }
    }
    
    void GLRenderer::gfxRegisterSysutilCallback()
    {
        // Register sysutil exit callback
        int ret = cellSysutilRegisterCallback(0, gfxSysutilCallback, NULL);
        if ( ret != CELL_OK ) {
            printf( "Registering sysutil callback failed...: error=0x%x\n", ret );
            exit(1);
        }
    }
#endif
    
    
    GLRenderer::GLRenderer(PE::GameContext &ctx, unsigned int width, unsigned int height)
		: IRenderer(ctx, width, height)
		, m_pContext(&ctx)
#if APIABSTRACTION_IOS
    , context(NULL)
    
#endif
    {
        m_width = width;
        m_height = height;
        initialiseContext();
    }
    
    void GLRenderer::initialiseContext()
    {
#if APIABSTRACTION_IOS
        
        EAGLContext *aContext = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
        
        if (!aContext)
        {
            aContext = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES1];
        }
        
        if (!aContext)
            NSLog(@"Failed to create ES context");
        else if (![EAGLContext setCurrentContext:aContext])
            NSLog(@"Failed to set ES context current");
        
        context = aContext;
        [aContext release];
        
        
        UIScreen *screen = [UIScreen mainScreen]; // get ptr to main screen
        CGRect bounds = [screen bounds];
        
        PEINFO("mainScreen bounds are x: %.2f y: %.2f w: %.2f h: %.2f\n", bounds.origin.x, bounds.origin.y, bounds.size.width, bounds.size.height);
    
        
        GLKView *baseView = [[[GLKView alloc] initWithFrame: bounds] autorelease];
        
        baseView.context = context;
        baseView.drawableDepthFormat = GLKViewDrawableDepthFormat24;
        baseView.multipleTouchEnabled = true;
        
        printf("self.view: %p\n", baseView);
        
        m_width = bounds.size.width;
        m_height = bounds.size.height;
        
        s_pViewController.view = baseView;
        
#endif
        
#if APIABSTRACTION_GLPC
		
		WinApplication *pWinApp = static_cast<WinApplication *>(m_pContext->getApplication());
        m_hdc = GetWindowDC(pWinApp->m_windowHandle);
		PIXELFORMATDESCRIPTOR pfd;
		memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));
		pfd.nSize  = sizeof(PIXELFORMATDESCRIPTOR);
		pfd.nVersion   = 1;
		pfd.dwFlags    = PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW;
		pfd.iPixelType = PFD_TYPE_RGBA;
		pfd.cColorBits = 32;
		pfd.cDepthBits = 32;
		pfd.iLayerType = PFD_MAIN_PLANE;

		int nPixelFormat = ChoosePixelFormat(m_hdc, &pfd);

		PEASSERT(nPixelFormat != 0, "Error choosing pixel format");

		BOOL bResult = SetPixelFormat (m_hdc, nPixelFormat, &pfd);

		PEASSERT(bResult != 0, "Error setting pixel format");

		HGLRC tempContext = wglCreateContext(m_hdc);
		wglMakeCurrent(m_hdc, tempContext);

		GLenum glewerr = glewInit();
		if (GLEW_OK != glewerr)
		{
			PEERROR("GLEW is not initialized!\n");
		}

		int attribs[] =
		{
			WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
			WGL_CONTEXT_MINOR_VERSION_ARB, 1,
			
			//WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB,
			//WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,                

// 			WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
// 			WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
// 			WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
// 			WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,

			//WGL_COLOR_BITS_ARB, 32,
			//WGL_DEPTH_BITS_ARB, 32,
			//WGL_STENCIL_BITS_ARB, 8,
			WGL_CONTEXT_FLAGS_ARB, 0,
			0
		};

		m_hglrc = wglCreateContextAttribsARB(m_hdc,0, attribs);
		wglMakeCurrent(NULL,NULL);
		wglDeleteContext(tempContext);
		wglMakeCurrent(m_hdc, m_hglrc);

		//Checking GL version
		const GLubyte *GLVersionString = glGetString(GL_VERSION);

		//Or better yet, use the GL3 way to get the version number
		int OpenGLVersion[2];
		glGetIntegerv(GL_MAJOR_VERSION, &OpenGLVersion[0]);
		glGetIntegerv(GL_MINOR_VERSION, &OpenGLVersion[1]);

		PEASSERT(m_hglrc, "Error creating gl context");


		char *args[]= {"executable.exe"};
		int numArgs = 1;

        glClearColor(0,0,0,0);
        glClearDepth(1.0f);

		IRenderer::checkForErrors("glutinit\n");

		m_hglrc = wglGetCurrentContext();
		m_hdc = wglGetCurrentDC();
		bool noError = wglMakeCurrent(m_hdc, m_hglrc);
		assert(noError);
		// Setup Cg
        
        m_cgContext = cgCreateContext();
        
		IRenderer::checkForErrors("cgCreateContext");

        if (m_cgContext == NULL)
        {
            MessageBoxA(NULL, "Failed To Create Cg Context", "Error", MB_OK);
            assert(!"Failed To Create Cg Context");
        }

        cgGLSetDebugMode(CG_TRUE);
		//? do we need this ?
		cgSetParameterSettingMode(m_cgContext, CG_IMMEDIATE_PARAMETER_SETTING/* CG_DEFERRED_PARAMETER_SETTING*/);
        //////////////////////////////////////////////////////////////////////////
       
		
        m_cgVertexProfile = cgGLGetLatestProfile(CG_GL_VERTEX);				// Get The Latest GL Vertex Profile

		IRenderer::checkForErrors("cgGLGetLatestProfile");

        // Validate Our Profile Determination Was Successful
        if (m_cgVertexProfile == CG_PROFILE_UNKNOWN)
        {
            MessageBoxA(NULL, "Invalid profile type", "Error", MB_OK);
            assert(!"Invalid profile type");
        }
        
        cgGLSetOptimalOptions(m_cgVertexProfile);
        
        IRenderer::checkForErrors("selecting vertex profile");
        
        m_cgFragmentProfile =  cgGLGetLatestProfile(CG_GL_FRAGMENT);
        cgGLSetOptimalOptions(m_cgFragmentProfile);
        
		IRenderer::checkForErrors("selecting fragment profile");
        
        cgGLEnableProfile(m_cgVertexProfile);
        IRenderer::checkForErrors("enabling vertex profile");
        
        cgGLEnableProfile(m_cgFragmentProfile);
        IRenderer::checkForErrors("enabling fragment profile");
#endif
        
#if APIABSTRACTION_PS3
        // PS3 initialization
        // Load required prx modules.
        int ret = cellSysmoduleLoadModule(CELL_SYSMODULE_GCM_SYS);
        switch( ret )
        {
            case CELL_OK:
                // The module is successfully loaded,
                break;
                
            case CELL_SYSMODULE_ERROR_DUPLICATED:
                // The module was already loaded,
                break;
                
            case CELL_SYSMODULE_ERROR_UNKNOWN:
            case CELL_SYSMODULE_ERROR_FATAL:
                printf("!! Failed to load CELL_SYSMODULE_GCM_SYS\n" ); 
                printf("!! Exiting Program \n" ); 
                exit(1);
        }
        
                    
        gfxRegisterSysutilCallback();
        
        // First, initialize PSGL
        // Note that since we initialized the SPUs ourselves earlier we should
        // make sure that PSGL doesn't try to do so as well.
        PSGLinitOptions initOpts={
        enable: PSGL_INIT_MAX_SPUS | PSGL_INIT_INITIALIZE_SPUS | PSGL_INIT_HOST_MEMORY_SIZE,
        maxSPUs: 1,
        initializeSPUs: false,
            // We're not specifying values for these options, the code is only here
            // to alleviate compiler warnings.
        persistentMemorySize: 0,
        transientMemorySize: 0,
        errorConsole: 0,
        fifoSize: 0,	
        hostMemorySize: 64* 1024*1024,  // 128 mbs for host memory 
        };
        
        psglInit(&initOpts);
        
        static PSGLdevice* device=NULL;
        device=psglCreateDeviceAuto(GL_ARGB_SCE,GL_DEPTH_COMPONENT24,GL_MULTISAMPLING_NONE_SCE);
        //device=psglCreateDeviceAuto(GL_ARGB_SCE,GL_DEPTH_COMPONENT24,GL_MULTISAMPLING_2X_DIAGONAL_CENTERED_SCE);
        //device=psglCreateDeviceAuto(GL_ARGB_SCE,GL_DEPTH_COMPONENT24,GL_MULTISAMPLING_4X_SQUARE_ROTATED_SCE);
        
        if ( !device )
        {
            printf("!! Failed to init the device \n" ); 
            printf("!! Exiting Program \n" ); 
            exit(1); 
        }
        psglGetDeviceDimensions(device,&m_width,&m_height);
        
        printf("gfxInitGraphics::PSGL Device Initialized Width %d Height %d \n",m_width, m_height ); 	
        
        GLfloat gfxAspectRatio = psglGetDeviceAspectRatio(device);
        
        // Now create a PSGL context
        PSGLcontext *pContext=psglCreateContext();
        
        
        if (pContext==NULL) {
            fprintf(stderr, "Error creating PSGL context\n");
            exit(-1);
        }
        
        // Make this context current for the device we initialized
        psglMakeCurrent(pContext, device);
        
        m_cgContext = cgCreateContext();
        
        if (m_cgContext == NULL)
        {
            assert(!"Failed To Create Cg Context");
        }
         
        // Reset the context
        psglResetCurrentContext();
        
        m_cgVertexProfile = cgGLGetLatestProfile(CG_GL_VERTEX);				// Get The Latest GL Vertex Profile
        cgGLSetOptimalOptions(m_cgVertexProfile);
        
        IRenderer::checkForErrors("selecting vertex profile");
        
        m_cgFragmentProfile =  cgGLGetLatestProfile(CG_GL_FRAGMENT);
        cgGLSetOptimalOptions(m_cgFragmentProfile);
        
        IRenderer::checkForErrors("selecting fragment profile");
        
        cgGLEnableProfile(m_cgVertexProfile);
        IRenderer::checkForErrors("enabling vertex profile");
        
        cgGLEnableProfile(m_cgFragmentProfile);
        
        glViewport(0, 0, m_width, m_height);
        glScissor(0, 0, m_width, m_height);
        glClearColor(0.f, 0.f, 0.f, 1.f);
        glEnable(GL_DEPTH_TEST);

		//to enable vsync:
		//glEnable(GL_VSYNC_SCE);
		glDisable(GL_VSYNC_SCE);
        
        // PSGL doesn't clear the screen on startup, so let's do that here.
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        psglSwap();
#endif // #if APIABSTRACTION_PS3
    }
    
    void GLRenderer::uninitialiseContext()
    {
#if APIABSTRACTION_IOS
        // Tear down context.
        if ([EAGLContext currentContext] == context)
            [EAGLContext setCurrentContext:nil];
        context = nil;
#endif
    }

	void GLRenderer::setVSync(bool useVsync)
	{
        #if PE_PLAT_IS_WIN32
            wglSwapIntervalEXT((int)(useVsync));
        #elif PE_PLAT_IS_PS3
            glDisable(GL_VSYNC_SCE);
        #endif
	}
    
    void GLRenderer::swap(bool vsync /* = 0*/) {
        #if APIABSTRACTION_PS3
            psglSwap();
        #elif APIABSTRACTION_IOS

        #else
            SwapBuffers(m_hdc);
        #endif
    }
    
    void GLRenderer::clear(){
        PEINFO("Need to implement clear()");
        //glClear(;
    }
    
	void GLRenderer::endFrame()
	{

	}

    const char* GLRenderer::outputGLBoolean(const char *prefix, GLenum glenum, const char* postfix)
    {
        if (s_debugGfx)
        {
            GLboolean enabled = GL_FALSE;
#if defined(SN_TARGET_PS3)
            sprintf(s_dbgStr, "Value unavailable on PS3");
#else
            glGetBooleanv(glenum, &enabled);
            sprintf(s_dbgStr, "%s%s%s", prefix, enabled ? "GL_TRUE" : "GL_FALSE", postfix);
#endif
            
            PEINFOSTR(s_dbgStr);
        }
        return s_dbgStr;
    }
    
	bool GLRenderer::checkForErrors(const char *situation)
	{
#if APIABSTRACTION_OGL
		GLenum glErr;

		glErr = glGetError();
		if (glErr != GL_NO_ERROR)
		{

			PEINFO("glError: %s\n", (const char*)(gluErrorString(glErr)));
		}

#if !APIABSTRACTION_IOS
		//OGL_GPUScreen *pGLRenderer = static_cast<OGL_GPUScreen *>(this);
		CGerror error;
		const char *string = cgGetLastErrorString(&error);

		if (error != CG_NO_ERROR)
        {
			PEINFO("%s\n", string);
			if (error == CG_COMPILER_ERROR)
            {
				const char *err = cgGetLastListing(m_cgContext);
				int l = StringOps::length(err);
				PEINFO("%s\n", err);
			}
			return false;
		}
#endif        
#endif
		return true;
	}


    const char* GLRenderer::outputGLint(const char *prefix, GLenum glenum, const char* postfix)
    {
        
        if (s_debugGfx)
        {
            GLint val = 0;
            glGetIntegerv(glenum, &val);
            
            PEINFO("%s%d%s", prefix, val, postfix);
        }
        return s_dbgStr;
    }
    
// IRenderer static method implementation

void IRenderer::Construct(PE::GameContext &context, unsigned int width, unsigned int height)
{
	PE::Handle h("OGL_GPUScreen", sizeof(GLRenderer));
	GLRenderer *pScreen = new(h) GLRenderer(context, width, height);
	context.m_pGPUScreen = pScreen;
}

bool IRenderer::checkForErrors(const char *situation)
{
#if APIABSTRACTION_OGL
	GLenum glErr;

	glErr = glGetError();
	if (glErr != GL_NO_ERROR)
	{
		PEWARN("%s: glError: %s \n", situation, (const char*)(gluErrorString(glErr)));
	}

#if !APIABSTRACTION_IOS
	//OGL_GPUScreen *pGLRenderer = static_cast<OGL_GPUScreen *>(this);
	CGerror error;
	const char *string = cgGetLastErrorString(&error);

	if (error != CG_NO_ERROR) {
		PEINFO("%s in situatuon: %s\n", string, situation);
		if (error == CG_COMPILER_ERROR) {
			//PEINFO("%s\n", cgGetLastListing(pGLRenderer->m_cgContext));
		}
		return false;
	}
#endif        
#endif
	return true;
}

void IRenderer::checkRenderBufferComplete()
{
	#if PE_PLAT_IS_IOS
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			NSLog(@"Failed to make complete framebuffer object %x", glCheckFramebufferStatus(GL_FRAMEBUFFER));
	#endif
}

void IRenderer::AcquireRenderContextOwnership(int &threadOwnershipMask)
{
	bool needAssert = (threadOwnershipMask & Threading::RenderContext) > 0;

	if (needAssert)
	{
		assert(!needAssert);
	}

	m_renderLock.lock();

#if APIABSTRACTION_GLPC
	GLRenderer *pGLRenderer = static_cast<GLRenderer *>(this);
	
	while (1)
	{
		bool noError = wglMakeCurrent(pGLRenderer->m_hdc, pGLRenderer->m_hglrc);
		//assert(noError);
		checkForErrors("Acquire ownership");
		if (noError)
			break;
	}
#endif

	threadOwnershipMask = threadOwnershipMask | Threading::RenderContext;
}

void IRenderer::ReleaseRenderContextOwnership(int &threadOwnershipMask)
{
	assert((threadOwnershipMask & Threading::RenderContext));

	m_renderLock.unlock();

#if APIABSTRACTION_GLPC
	bool noError = wglMakeCurrent(0, 0);
	assert(noError);
#endif

	threadOwnershipMask = threadOwnershipMask & ~Threading::RenderContext;
}



}; // namespace PE

