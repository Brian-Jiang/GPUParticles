#ifndef __PYENGINE_2_0_GPUSCREEN_H___
#define __PYENGINE_2_0_GPUSCREEN_H___

#define NOMINMAX
// APIAbstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

// Outer-Engine includes

// Inter-Engine includes
#include "PrimeEngine/MemoryManagement/Handle.h"
#include "PrimeEngine/Math/Vector4.h"
#include "PrimeEngine/Game/Common/GameContext.h"
#include "PrimeEngine/APIAbstraction/Threading/Threading.h"
#include "PrimeEngine/Utils/PEClassDecl.h"
// Sibling/Children includes

namespace PE {
	class D3D11Renderer;
	class D3D9Renderer;
	class GLRenderer;
	struct TextureGPU;
class IRenderer : public PE::PEAllocatableAndDefragmentable
{
public:
	IRenderer(PE::GameContext &context, unsigned int width, unsigned int height);
	virtual ~IRenderer(){}
	// api specific classes will define this function
	static void Construct(PE::GameContext &context, unsigned int width, unsigned int height);
	
	virtual void swap(PrimitiveTypes::Bool vsync = false) = 0;

	virtual PrimitiveTypes::UInt32 getWidth() = 0;
	virtual PrimitiveTypes::UInt32 getHeight() = 0;

	virtual void setClearColor(Vector4 color) = 0;
	virtual void setVSync(bool useVsync) = 0;

	virtual void clear() = 0;

	static bool checkForErrors(const char *situation);

	static void checkRenderBufferComplete();

	void AcquireRenderContextOwnership(int &threadOwnershipMask);
	void ReleaseRenderContextOwnership(int &threadOwnershipMask);

	virtual void setRenderTargetsAndViewportWithNoDepth(TextureGPU *pDestColorTex = 0, bool clear = false) = 0;
	virtual void setRenderTargetsAndViewportWithDepth(TextureGPU *pDestColorTex = 0, TextureGPU *pDestDepthTex = 0, bool clearRenderTargte = false, bool clearDepth = false) = 0;
	virtual void setDepthStencilOnlyRenderTargetAndViewport(TextureGPU *pDestDepthTex, bool clear = false) = 0;

	virtual void endRenderTarget(TextureGPU *pTex) = 0;
	virtual void endFrame() = 0;
	

private:
	Vector4 m_clearColor;
	Threading::Mutex m_renderLock;
	char m_vsProfile[32];
	char m_psProfile[32];
public:
	
	enum RenderMode
	{
		RenderMode_DefaultGlow = 0,
		RenderMode_DefaultNoPostProcess,
		RenderMode_DebugGlowRT,
		RenderMode_DebugSeparatedGlow,
		RenderMode_DebugGlowHorizontalBlur,
		RenderMode_DebugGlowVerticalBlurCombine,
		RenderMode_DebugShadowRT,
		RenderMode_Count
	};

	RenderMode m_renderMode;

};

}; // namespace PE

#endif
