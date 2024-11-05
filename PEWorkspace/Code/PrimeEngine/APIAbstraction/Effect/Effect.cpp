#define NOMINMAX
#include "Effect.h"

#include "EffectManager.h"
#include "PEAlphaBlendState.h"

#include "PrimeEngine/FileSystem/FileWriter.h"
#include "../../Lua/LuaEnvironment.h"
#include "../../Lua/EventGlue/EventDataCreators.h"
#include "PrimeEngine/Utils/ErrorHandling.h"

#include "PrimeEngine/APIAbstraction/GPUBuffers/BufferInfo.h"
#include "PrimeEngine/APIAbstraction/GPUBuffers/VertexBufferGPU.h"

#include "PrimeEngine/Game/Common/GameContext.h"

namespace PE {

namespace Components {

PE_IMPLEMENT_CLASS1(Effect, Component);

void effectLoadThreadFunction(void *params)
{
	{
		Effect *pEffect = (Effect *)(params);
		API_CHOOSE_DX11_DX9_OGL_PSVITA(pEffect->loadTechniqueSync_D3D11(), pEffect->loadTechniqueSync_D3D9(), pEffect->loadTechniqueSync_GL(), pEffect->loadTechniqueSync_PSVita());
	}
	return;
}

Effect::Effect(PE::GameContext &context, PE::MemoryArena arena, PE::Handle hMyself) : Component(context, arena, hMyself), m_isReady(false)
#if APIABSTRACTION_D3D9
    , m_pInputLayout(NULL)
#endif
    , m_effectDrawOrder(EffectDrawOrder::First)
	, m_pBlendState(NULL)
	, m_pRasterizerState(NULL)
    , m_pDepthStencilState(NULL)
	, m_externalPerTechniqueData(context, arena)
{
	m_arena = arena; m_pContext = &context;
	m_vsVertexFormat = PEVertexFormat_Count;
#if PE_PLAT_IS_PSVITA
	memset(&m_basicVertexProgram[0], 0, sizeof(m_basicVertexProgram));
#endif
}
    
void Effect::readIntoBufferReplaceEOL(PE::GameContext &context, char *&pdata, const char *filename)
{
	
	char path[128];
#	if APIABSTRACTION_D3D9 || APIABSTRACTION_D3D11 || APIABSTRACTION_OGL
	StringOps::concat(context.getMainFunctionArgs()->gameProjRoot(), "AssetsOut\\Default\\GPUPrograms\\", path, 128);
#	endif

	StringOps::concat(path, filename, path, 128);
	FileReader f(path);
	PrimitiveTypes::UInt32 filesize = 0;
	f.readIntoBuffer(pdata, filesize);

}

void Effect::writeEffectFile(PE::GameContext &context, const char *filename, const char *ptext)
{
	char path[128];
#	if APIABSTRACTION_D3D9 || APIABSTRACTION_D3D11 || APIABSTRACTION_OGL
	StringOps::concat(context.getMainFunctionArgs()->gameProjRoot(), "AssetsOut\\Default\\GPUPrograms\\", path, 128);
#	endif

	StringOps::concat(path, filename, path, 128);

	FileWriter f(path);
	f.writeString(ptext);
}


void Effect::loadFromCopy(Effect &cpy)
{
	lock();
	cpy.lock();

#if APIABSTRACTION_D3D9
	m_pInputLayout = cpy.m_pInputLayout;
	m_VS = cpy.m_VS;
	m_PS = cpy.m_PS;
#elif APIABSTRACTION_D3D11
	m_VS = cpy.m_VS;
	m_PS = cpy.m_PS;
	m_GS = cpy.m_GS;
	m_CS = cpy.m_CS;
#elif APIABSTRACTION_OGL
	m_VS = cpy.m_VS;
	m_PS = cpy.m_PS;
	m_program = cpy.m_program;
	#if APIABSTRACTION_IOS
		m_cgVertexProgram = cpy.m_cgVertexProgram;
		m_cgFragmentProgram = cpy.m_cgFragmentProgram;
		m_glslProgram = cpy.m_glslProgram;
	#else
		m_cgVertexProgram = cpy.m_cgVertexProgram;
		m_cgFragmentProgram = cpy.m_cgFragmentProgram;
	#endif
#elif PE_PLAT_IS_PSVITA
	
#endif



	m_SpeS = cpy.m_SpeS;

	StringOps::writeToString(cpy.m_techName, m_techName, 256);

	StringOps::writeToString(cpy.m_vsName, m_vsName, 256);
	cpy.m_vsVertexFormat = m_vsVertexFormat;
	StringOps::writeToString(cpy.m_vsFilename, m_vsFilename, 256);

	StringOps::writeToString(cpy.m_psName, m_psName, 256);
	StringOps::writeToString(cpy.m_psFilename, m_psFilename, 256);

	StringOps::writeToString(cpy.m_gsName, m_gsName, 256);
	StringOps::writeToString(cpy.m_gsFilename, m_gsFilename, 256);

	StringOps::writeToString(cpy.m_csName, m_csName, 256);
	StringOps::writeToString(cpy.m_csFilename, m_csFilename, 256);

	StringOps::writeToString(cpy.m_spesName, m_spesName, 256);
	StringOps::writeToString(cpy.m_spesFilename, m_spesFilename, 256);

    m_pRasterizerState = cpy.m_pRasterizerState;
    m_pBlendState = cpy.m_pBlendState;
    m_pDepthStencilState = cpy.m_pDepthStencilState;
    
	m_isReady = cpy.m_isReady;

	unlock();
	cpy.unlock();
}

void Effect::setPixelShader(Effect &cpy)
{
	lock();
	cpy.lock();
#if APIABSTRACTION_D3D9 || APIABSTRACTION_D3D11
	m_PS = cpy.m_PS;
#elif APIABSTRACTION_OGL
	m_PS = cpy.m_PS;
	m_cgFragmentProgram = cpy.m_cgFragmentProgram;
#elif PE_PLAT_IS_PSVITA
	m_basicFragmentProgramId = cpy.m_basicFragmentProgramId;
	m_basicFragmentProgram = cpy.m_basicFragmentProgram;
#endif
	StringOps::writeToString(cpy.m_psName, m_psName, 256);
	StringOps::writeToString(cpy.m_psFilename, m_psFilename, 256);

	unlock();
	cpy.unlock();
}

void Effect::loadTechnique(
	const char *vsFilenameNoExt, const char *vsName, 
	const char *gsFilenameNoExt, const char *gsName,
	const char *psFilenameNoExt, const char *psName,
	const char *csFilenameNoExt, const char *csName,
	E_PERasterizerState rasterizerStateId,
	E_PEDepthStencilState depthStencilStateId,
    E_PEAlphaBlendState blendStateId,
    const char *techniqueName,
	bool reload)
{
	const char *vsFilename = NULL;
	const char *gsFilename = NULL;
	const char *psFilename = NULL;
	const char *csFilename = NULL;

	char vsFilenameBuf[256];
	char gsFilenameBuf[256];
	char psFilenameBuf[256];
	char csFilenameBuf[256];

	if (vsFilenameNoExt){ sprintf(vsFilenameBuf, "%s.%s", vsFilenameNoExt, API_CHOOSE_PS3_PSVITA_ELSE("cgelf", "gxp", "cgvs")); vsFilename = vsFilenameBuf;}
	if (gsFilenameNoExt){ sprintf(gsFilenameBuf, "%s.%s", gsFilenameNoExt, API_CHOOSE_PS3_PSVITA_ELSE("cgelf", "gxp", "cggs")); gsFilename = gsFilenameBuf;}
	if (psFilenameNoExt){ sprintf(psFilenameBuf, "%s.%s", psFilenameNoExt, API_CHOOSE_PS3_PSVITA_ELSE("cgelf", "gxp", "cgps")); psFilename = psFilenameBuf;}
	if (csFilenameNoExt){ sprintf(csFilenameBuf, "%s.%s", csFilenameNoExt, API_CHOOSE_PS3_PSVITA_ELSE("cgelf", "gxp", "cgcs")); csFilename = csFilenameBuf;}


	//validate vertex shader name:
	if (vsName != 0)
	{
		m_vsVertexFormat = getFormatFromVSName(vsFilename);
		
		if (m_vsVertexFormat == PEVertexFormat_Count)
		{
			PEASSERT(false, "VertexShader %s name does not conform to naming notation. It has to start with vertex buffer format followed by _", vsFilename);
		}
	}

	if (csFilename)
	{
		PEASSERT(vsFilename == psFilename && vsFilename == 0, "Compute shaders have to be the only shader in technique");
	}
	else
	{
		m_pBlendState = m_pContext->getAlphaBlendStateManager()->getAlphaBlendState(blendStateId);
		m_pRasterizerState = m_pContext->getRasterizerStateManager()->getRasterizerState(rasterizerStateId);
		m_pDepthStencilState = m_pContext->getDepthStencilStateManager()->getDepthStencilState(depthStencilStateId);
	}

	#if APIABSTRACTION_D3D9
		PEASSERT(gsFilename == 0 && csFilename == 0, "Dont support this shaders on this platform");
		loadTechnique_D3D9_VS_PS(vsFilename, vsName, psFilename, psName, techniqueName, reload);
	#elif APIABSTRACTION_D3D11
		loadTechnique_D3D11(vsFilename, vsName, gsFilename, gsName, psFilename, psName, csFilename, csName, techniqueName, 	reload);
	#elif APIABSTRACTION_OGL
		PEASSERT(gsFilename == 0 && csFilename == 0, "Dont support this shaders on this platform");
		loadTechnique_GL(vsFilename, vsName, psFilename, psName, techniqueName, reload);
	#elif PE_PLAT_IS_PSVITA
		PEASSERT(gsFilename == 0 && csFilename == 0, "Dont support this shaders on this platform");
		loadTechnique_PSVita(vsFilename, vsName, psFilename, psName, techniqueName, reload);
	#endif
	
	
}


void Effect::loadTechniqueAsync()
{
	if (StringOps::length(m_spesFilename) == 0)
	{
#if 1 // force synchronous load for now..
		API_CHOOSE_DX11_DX9_OGL_PSVITA(loadTechniqueSync_D3D11(), loadTechniqueSync_D3D9(), loadTechniqueSync_GL(), loadTechniqueSync_PSVita());
#		else
			// spawn a thread to load technique
			HANDLE loadThread =(HANDLE)_beginthreadex(NULL,
				1024,
				effectLoadThreadFunction,
				this,
				0, //CREATE_SUSPENDED,
				0);
#		endif
	}
	else
	{
		// SPE shaders are loaded synchronously
		API_CHOOSE_DX11_DX9_OGL_PSVITA(loadTechniqueSync_D3D11(), loadTechniqueSync_D3D9(), loadTechniqueSync_GL(), loadTechniqueSync_PSVita());
	}
}

void Effect::setCurrent(PE::VertexBufferGPU *pCurVertexBuffer)
{
	/*if (!lock())
		return;*/

	if (!m_isReady)
		return;
#if PE_PLAT_IS_PSVITA
	

#elif APIABSTRACTION_D3D9
	{
		IDirect3DVertexShader9 *pVertexShader = m_VS;
		IDirect3DPixelShader9 *pPixelShader = m_PS;

		D3D9Renderer *pD3D9Renderer = static_cast<D3D9Renderer *>(m_pContext->getGPUScreen());
		LPDIRECT3DDEVICE9 pDevice = pD3D9Renderer->m_pD3D9Device;
		
		pDevice->SetVertexShader(pVertexShader);

		pDevice->SetPixelShader(pPixelShader);

		pDevice->SetRenderState( D3DRS_LIGHTING, FALSE );

		PEASSERT(m_pBlendState != NULL, "Every Technique has to have a blend state");
		m_pBlendState->bindToPipeline(this);

		PEASSERT(m_pRasterizerState != NULL, "Every Technique has to have a rasterizer state");
		m_pRasterizerState->bindToPipeline(this);
        
        PEASSERT(m_pDepthStencilState != NULL, "Every Technique has to have a depth stencil state");
        m_pDepthStencilState->bindToPipeline(this);
	}

#	elif APIABSTRACTION_D3D11

	D3D11Renderer *pD3D11Renderer = static_cast<D3D11Renderer *>(m_pContext->getGPUScreen());
	ID3D11Device *pDevice = pD3D11Renderer->m_pD3DDevice;
	ID3D11DeviceContext *pDeviceContext = pD3D11Renderer->m_pD3DContext;

	pDeviceContext->VSSetShader(m_VS, NULL, 0);
	pDeviceContext->GSSetShader(m_GS, NULL, 0);
	pDeviceContext->PSSetShader(m_PS, NULL, 0);
	pDeviceContext->CSSetShader(m_CS, NULL, 0);

	PEASSERT(m_CS || m_pBlendState != NULL, "Every Non-CS Technique has to have a blend state");
	if (m_pBlendState)
		m_pBlendState->bindToPipeline(this);

	PEASSERT(m_CS || m_pRasterizerState != NULL, "Every Non-CS Technique has to have a rasterizer state");
	if (m_pRasterizerState)
		m_pRasterizerState->bindToPipeline(this);

    PEASSERT(m_CS || m_pDepthStencilState != NULL, "Every Non-CS Technique has to have a depth stencil state");
	if (m_pDepthStencilState)
		m_pDepthStencilState->bindToPipeline(this);

#elif APIABSTRACTION_OGL

	#if APIABSTRACTION_IOS
        glUseProgram(m_glslProgram);
    
        GLint logLength, status;
    
        glValidateProgram(m_glslProgram);
        glGetProgramiv(m_glslProgram, GL_INFO_LOG_LENGTH, &logLength);
        if (logLength > 0)
        {
            GLchar *log = (GLchar *)malloc(logLength);
            glGetProgramInfoLog(m_glslProgram, logLength, &logLength, log);
            NSLog(@"Program validate log:\n%s", log);
            free(log);
        }
    
        glGetProgramiv(m_glslProgram, GL_VALIDATE_STATUS, &status);
        assert(status);
    #else
        cgGLBindProgram(m_cgVertexProgram);
        IRenderer::checkForErrors("binding vertex program");

        cgGLBindProgram(m_cgFragmentProgram);
        IRenderer::checkForErrors("binding fragment program");

        cgGLEnableProfile(cgGLGetLatestProfile(CG_GL_VERTEX));
        cgGLEnableProfile(cgGLGetLatestProfile(CG_GL_FRAGMENT));
    #endif

	PEASSERT(m_pRasterizerState != NULL, "Every Technique has to have a rasterizer state");
	m_pRasterizerState->bindToPipeline(this);

	PEASSERT(m_pBlendState != NULL, "Every Technique has to have a blend state");
	m_pBlendState->bindToPipeline(this);
    
    PEASSERT(m_pDepthStencilState != NULL, "Every Technique has to have a depth stencil state");
	m_pDepthStencilState->bindToPipeline(this);

	
#endif
	// unlock();
}

#if APIABSTRACTION_D3D11
void Effect::setConstantBuffer(ID3D11Device *pDevice, ID3D11DeviceContext *pDeviceContex, ID3D11Buffer * pBuf, PrimitiveTypes::UInt32 slotId, void *pVal, PrimitiveTypes::UInt32 size)
{			
	/*
			typedef enum D3D11_MAP {
				D3D11_MAP_READ                 = 1,
				D3D11_MAP_WRITE                = 2,
				D3D11_MAP_READ_WRITE           = 3,
				D3D11_MAP_WRITE_DISCARD        = 4,
				D3D11_MAP_WRITE_NO_OVERWRITE   = 5 
			} D3D11_MAP; */

			
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	HRESULT hr = pDeviceContex->Map(
		pBuf, // resource
		0, // subresource
		D3D11_MAP_WRITE_DISCARD, 
		0, 
		&mappedResource);
	assert(SUCCEEDED(hr));
	// set value
	memcpy(mappedResource.pData, pVal, size);

	pDeviceContex->Unmap(pBuf, 0);

	pDeviceContex->CSSetConstantBuffers(slotId, 1, &pBuf);

	pDeviceContex->VSSetConstantBuffers(slotId, 1, &pBuf);
	pDeviceContex->GSSetConstantBuffers(slotId, 1, &pBuf);
	pDeviceContex->PSSetConstantBuffers(slotId, 1, &pBuf);
}
#endif

}; // namespace Components
}; // namespace PE
