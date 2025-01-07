#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"
#include "RenderJob.h"
#include "PrimeEngine/Scene/DrawList.h"

#if APIABSTRACTION_IOS
#import <QuartzCore/QuartzCore.h>
#import <UIKit/UIKit.h>
#import <CoreData/CoreData.h>
#endif

extern "C"{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
};

#include "PrimeEngine/Game/Client/ClientGame.h"

namespace PE {

using namespace Events;
using namespace Components;

void drawThreadFunctionJob(void *params)
{
	GameContext *pContext = static_cast<PE::GameContext *>(params);

	g_drawThreadInitializationLock.lock();
	//initialization here..
	g_drawThreadInitialized = true;
	g_drawThreadExited = false;
	g_drawThreadInitializationLock.unlock();

	//acquire rendring thread lock so that we can sleep on it until game thread wakes us up
	g_drawThreadLock.lock();

	// now we can signal main thread that this thread is initialized
	g_drawThreadInitializedCV.signal();
    while (1)
    {
		runDrawThreadSingleFrameThreaded(*pContext);
		if (g_drawThreadExited)
			return;
    }
    return;
}

void runDrawThreadSingleFrameThreaded(PE::GameContext &ctx)
{
	while (!g_drawThreadCanStart && !g_drawThreadShouldExit)
    {
		bool success = g_drawCanStartCV.sleep();
        assert(success);
    }
	g_drawThreadCanStart = false; // set to false for next frame

	//PEINFO("Draw thread got g_drawThreadLock\n");

	if (g_drawThreadShouldExit)
	{
		//right now game thread is waiting on this thread to finish
		g_drawThreadLock.unlock();
		g_drawThreadExited = true;
		return;
	}

	runDrawThreadSingleFrame(ctx);
}

void runDrawThreadSingleFrame(PE::GameContext &ctx)
{
	int threadOwnershipMask = 0;
	
	ctx.getGPUScreen()->AcquireRenderContextOwnership(threadOwnershipMask);

	#if PE_ENABLE_GPU_PROFILING
	Timer t;
	PE::Profiling::Profiler::Instance()->startEventQuery(Profiling::Group_DrawThread, IRenderer::Instance()->getDevice(), t.GetTime(), "DrawThread");
	#endif

	#if APIABSTRACTION_D3D9
		//IRenderer::Instance()->getDevice()->SetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	#elif APIABSTRACTION_D3D11
		D3D11Renderer *pD3D11Renderer = static_cast<D3D11Renderer *>(ctx.getGPUScreen());
		ID3D11Device *pDevice = pD3D11Renderer->m_pD3DDevice;
		ID3D11DeviceContext *pDeviceContext = pD3D11Renderer->m_pD3DContext;

		pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	#endif

	bool renderShadowMap = false;
	#if !APIABSTRACTION_IOS && !APIABSTRACTION_PS3 && !PE_PLAT_IS_PSVITA /* && !PE_API_IS_D3D11*/
		renderShadowMap = true;
	#endif
	if (renderShadowMap)
	{
		// for shadow mapping:
		EffectManager::Instance()->setShadowMapRenderTarget();

		DrawList::ZOnlyInstanceReadOnly()->optimize();

		ctx.getGPUScreen()->ReleaseRenderContextOwnership(threadOwnershipMask);

		// the render context is acquired and release inside of this function
		DrawList::ZOnlyInstanceReadOnly()->do_RENDER_Z_ONLY(NULL, threadOwnershipMask);

		ctx.getGPUScreen()->AcquireRenderContextOwnership(threadOwnershipMask);

		EffectManager::Instance()->endCurrentRenderTarget();
	}

	IRenderer::checkForErrors("renderjob update start\n");

	IRenderer::RenderMode renderMode = ctx.getGPUScreen()->m_renderMode;
	bool disableScreenSpaceEffects = renderMode == IRenderer::RenderMode_DefaultNoPostProcess;
	if (!disableScreenSpaceEffects)
    {
		// set render target: GlowTargetTextureGPU
        EffectManager::Instance()->setTextureAndDepthTextureRenderTargetForGlow();
         
        assert(DrawList::InstanceReadOnly() != DrawList::Instance());
        DrawList::InstanceReadOnly()->optimize();
                
		// set global shader value (applied to all draw calls) for shadow map texture
		if (renderShadowMap)
			EffectManager::Instance()->createSetShadowMapShaderValue(DrawList::InstanceReadOnly());

		ctx.getGPUScreen()->ReleaseRenderContextOwnership(threadOwnershipMask);

		DrawList::InstanceReadOnly()->do_RENDER(NULL, threadOwnershipMask);
		ctx.getGPUScreen()->AcquireRenderContextOwnership(threadOwnershipMask);

		EffectManager::Instance()->endCurrentRenderTarget();

		#if APIABSTRACTION_D3D9
			//IRenderer::Instance()->getDevice()->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		#elif APIABSTRACTION_D3D11
			pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		#endif

		// sets render target to separated glow texture
		EffectManager::Instance()->drawGlowSeparationPass();
		EffectManager::Instance()->endCurrentRenderTarget();

        // First glow path into another texture with horizontal glow
                
        // Draw Effects
        // horizontal glow into 2nd glow target
        EffectManager::Instance()->drawFirstGlowPass();
		EffectManager::Instance()->endCurrentRenderTarget();

        // from second glow target to FinishedGlowTargetTexture

		EffectManager::Instance()->drawSecondGlowPass();
		EffectManager::Instance()->endCurrentRenderTarget();

		bool drawMotionBlur = renderMode == IRenderer::RenderMode_DefaultGlow;
		if (drawMotionBlur)
		{
			//draw back into main back buffer render target
			EffectManager::Instance()->drawMotionBlur();
			EffectManager::Instance()->endCurrentRenderTarget();
		}
		else
		{
			bool debugGlowRenderTarget = renderMode == IRenderer::RenderMode_DebugGlowRT;
			bool drawSeparatedGlow = renderMode == IRenderer::RenderMode_DebugSeparatedGlow;
			bool drawGlow1stPass = renderMode == IRenderer::RenderMode_DebugGlowHorizontalBlur;
			bool drawGlow2ndPass = renderMode == IRenderer::RenderMode_DebugGlowVerticalBlurCombine;
			bool drawShadowRenderTarget = renderMode == IRenderer::RenderMode_DebugShadowRT;

			EffectManager::Instance()->debugDrawRenderTarget(debugGlowRenderTarget, drawSeparatedGlow, drawGlow1stPass, drawGlow2ndPass, drawShadowRenderTarget);
			EffectManager::Instance()->endCurrentRenderTarget();
		}
    }
    else
    {
        // use simple rendering
        // the render target here is the same as end result of motion blur
        EffectManager::Instance()->setTextureAndDepthTextureRenderTargetForDefaultRendering();
                
        assert(DrawList::InstanceReadOnly() != DrawList::Instance());
                
		DrawList::InstanceReadOnly()->optimize();
		
		#if PE_PLAT_IS_PSVITA
			EffectManager::Instance()->drawFullScreenQuad();
		#endif
		
		// set global shader value (applied to all draw calls) for shadow map texture
		if (renderShadowMap)
			EffectManager::Instance()->createSetShadowMapShaderValue(DrawList::InstanceReadOnly());

		ctx.getGPUScreen()->ReleaseRenderContextOwnership(threadOwnershipMask);

        DrawList::InstanceReadOnly()->do_RENDER(NULL, threadOwnershipMask);

		ctx.getGPUScreen()->AcquireRenderContextOwnership(threadOwnershipMask);

        // Required when rendering to backbuffer directly. also done in drawMotionBlur_EndScene() since it is the last step of post process
		EffectManager::Instance()->endCurrentRenderTarget();
    }
            
	ctx.getGPUScreen()->endFrame();

    // Flip screen
	ctx.getGPUScreen()->swap(false);
    PE::IRenderer::checkForErrors("");

			
	#if PE_ENABLE_GPU_PROFILING
		Timer::TimeType time = t.TickAndGetCurrentTime();
		// only perform flush when gpu profiling is enabled, swap() above will call present which should flush when PE_DETAILED_GPU_PROFILING = 0 == vsync enabled.
		Profiling::Profiler::Instance()->update(Profiling::Group_DrawCalls, PE_DETAILED_GPU_PROFILING, true, time, t);
		Profiling::Profiler::Instance()->update(Profiling::Group_DrawThread, PE_DETAILED_GPU_PROFILING, true, time, t);
	#endif

	ctx.getGPUScreen()->ReleaseRenderContextOwnership(threadOwnershipMask);
}


}; // namespace PE

