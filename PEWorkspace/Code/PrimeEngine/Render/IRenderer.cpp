// APIAbstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

// Outer-Engine includes

// Inter-Engine includes

// Sibling/Children includes
#include "IRenderer.h"

namespace PE {
IRenderer::IRenderer(PE::GameContext &context, unsigned int width, unsigned int height) : m_clearColor(0,0,0,0)
{
	m_vsProfile[0] = '\0';
	m_psProfile[0] = '\0';

	m_renderMode = RenderMode_DefaultGlow;
	
#if APIABSTRACTION_IOS || APIABSTRACTION_PS3 || PE_PLAT_IS_PSVITA
	m_renderMode = RenderMode_DefaultNoPostProcess;
#endif

}

}; // namespace PE
