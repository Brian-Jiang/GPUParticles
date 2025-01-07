#define NOMINMAX
#include "Effect.h"

#include "EffectManager.h"

#include "PrimeEngine/FileSystem/FileWriter.h"
#include "../../Lua/LuaEnvironment.h"
#include "../../Lua/EventGlue/EventDataCreators.h"
#include "PrimeEngine/Utils/ErrorHandling.h"

#include "PrimeEngine/Game/Common/GameContext.h"


namespace PE {

namespace Components {

#if APIABSTRACTION_D3D11
	
void Effect::loadTechnique_D3D11(
		const char *vsFilename, const char *vsName, 
		const char *gsFilename, const char *gsName,
		const char *psFilename, const char *psName,
		const char *csFilename, const char *csName,
		const char *techniqueName,
		bool reload)
{
	lock();

	m_isReady = false;

	StringOps::writeToString(techniqueName, m_techName, 256);
	
	StringOps::writeToString(vsName ? vsName : "", m_vsName, 256);
	StringOps::writeToString(vsFilename ? vsFilename : "", m_vsFilename, 256);

	StringOps::writeToString(psName ? psName : "", m_psName, 256);
	StringOps::writeToString(psFilename ? psFilename : "", m_psFilename, 256);

	StringOps::writeToString(gsName ? gsName : "", m_gsName, 256);
	StringOps::writeToString(gsFilename ? gsFilename : "", m_gsFilename, 256);

	StringOps::writeToString(csName ? csName : "", m_csName, 256);
	StringOps::writeToString(csFilename ? csFilename : "", m_csFilename, 256);

	StringOps::writeToString("", m_spesName, 256);
	StringOps::writeToString("", m_spesFilename, 256);

	unlock();

	loadTechniqueAsync();
	
}


void Effect::loadTechniqueSpeS(
		const char *spesFilename, const char *spesName,
		const char *techniqueName)
{
	lock();

	m_isReady = false;

	StringOps::writeToString(techniqueName, m_techName, 256);
	
	StringOps::writeToString("", m_vsName, 256);
	StringOps::writeToString("", m_vsFilename, 256);

	StringOps::writeToString("", m_psName, 256);
	StringOps::writeToString("", m_psFilename, 256);

	StringOps::writeToString("", m_gsName, 256);
	StringOps::writeToString("", m_gsFilename, 256);

	StringOps::writeToString("", m_csName, 256);
	StringOps::writeToString("", m_csFilename, 256);

	StringOps::writeToString(spesFilename, m_spesFilename, 256);
	StringOps::writeToString(spesName, m_spesName, 256);

	unlock();

	loadTechniqueAsync();
	
}
// called in a separate thread
void Effect::loadTechniqueSync_D3D11()
{
	D3D11Renderer *pD3D11Renderer = static_cast<D3D11Renderer *>(m_pContext->getGPUScreen());
	ID3D11Device *pDevice = pD3D11Renderer->m_pD3DDevice;
	ID3D11DeviceContext *pDeviceContext = pD3D11Renderer->m_pD3DContext;

	lock();
	ID3D10Blob* pVertexShaderBuffer = NULL;
	ID3D10Blob* pGeometryShaderBuffer = NULL;
	ID3D10Blob* pPixelShaderBuffer = NULL;
	ID3D10Blob* pComputeShaderBuffer = NULL;

	ID3D11VertexShader *pVertexShader = NULL;
	ID3D11GeometryShader *pGeometryShader = NULL;
	ID3D11PixelShader *pPixelShader = NULL;
	ID3D11ComputeShader *pComputeShader = NULL;

	if (m_SpeS.isValid())
		m_SpeS.release();

	if (StringOps::length(m_spesFilename) == 0)
	{
		// don't have SPE Shader.

		D3D10_SHADER_MACRO Shader_Macros[3] = { {"HLSL_SEPARATE_LOAD", "1"} , {"APIABSTRACTION_D3D11", "1"}, {0, 0}};

		if (StringOps::length(m_vsFilename))
		{
			loadShader_D3D11(m_vsFilename, m_vsName, Shader_Macros,
				"vs_5_0", pVertexShaderBuffer);
		}

		if (StringOps::length(m_gsFilename))
		{
			loadShader_D3D11(m_gsFilename, m_gsName, Shader_Macros,
				"gs_5_0", pGeometryShaderBuffer);
		}

		if (StringOps::length(m_psFilename))
		{
			int existingIndex = EffectManager::Instance()->m_pixelShaders.findIndex(m_psName);
			/*
			if (existingIndex)
			{
				pPixelShader = EffectManager::Instance()->m_pixelShaders.m_pairs[existingIndex];
			}
			else
			*/
			{
				loadShader_D3D11(m_psFilename, m_psName, Shader_Macros,
					"ps_5_0", pPixelShaderBuffer);

				if (pPixelShaderBuffer)
				{
					HRESULT hr = pDevice->CreatePixelShader(pPixelShaderBuffer->GetBufferPointer(),
						pPixelShaderBuffer->GetBufferSize(), NULL, &pPixelShader);
					assert(SUCCEEDED(hr));

					pPixelShaderBuffer->Release();
					EffectManager::Instance()->m_pixelShaders.add(m_psName, pPixelShader);
				}
			}
		}

		if (StringOps::length(m_csFilename))
		{
			loadShader_D3D11(m_csFilename, m_csName, Shader_Macros,
				"cs_5_0", pComputeShaderBuffer);
		}

		HRESULT hr;

		if (pVertexShaderBuffer)
		{
			hr = pDevice->CreateVertexShader(pVertexShaderBuffer->GetBufferPointer(),
				pVertexShaderBuffer->GetBufferSize(), NULL, &pVertexShader);
			assert(SUCCEEDED(hr));
			
		}

		if (pGeometryShaderBuffer)
		{
			hr = pDevice->CreateGeometryShader(pGeometryShaderBuffer->GetBufferPointer(),
				pGeometryShaderBuffer->GetBufferSize(), NULL, &pGeometryShader);
			assert(SUCCEEDED(hr));
			pGeometryShaderBuffer->Release();
		}

		if (pComputeShaderBuffer)
		{
			hr = pDevice->CreateComputeShader(pComputeShaderBuffer->GetBufferPointer(),
				pComputeShaderBuffer->GetBufferSize(), NULL, &pComputeShader);
			assert(SUCCEEDED(hr));
			pComputeShaderBuffer->Release();
		}

		m_VS = pVertexShader;
		m_GS = pGeometryShader;
		m_PS = pPixelShader;
		m_CS = pComputeShader;

		m_isReady = true;
	} // if don't have Spe Shader
	else
	{
		// have Spe Shader
		m_isReady = false;
		loadSPEShader(m_spesFilename, m_spesName, m_SpeS);
	}

	m_externalPerTechniqueData.initVertexAttributeBindings(this);
	m_externalPerTechniqueData.init(this, pVertexShaderBuffer);
	if (pVertexShaderBuffer)
		pVertexShaderBuffer->Release();

	unlock();
}

void Effect::loadShader_D3D11(const char *filename, const char *name,
		D3D_SHADER_MACRO *pMacros,
		const char *version,
		ID3DBlob* &pShaderBuffer)
{
	
	DWORD dwShaderFlags = D3D10_SHADER_ENABLE_STRICTNESS;

#if defined( DEBUG ) || defined( _DEBUG )
	// Set the D3D10_SHADER_DEBUG flag to embed debug information in the shaders.
	// Setting this flag improves the shader debugging experience, but still allows 
	// the shaders to be optimized and to run exactly the way they will run in 
	// the release configuration of this program.
	dwShaderFlags |= D3D10_SHADER_DEBUG | D3D10_SHADER_SKIP_OPTIMIZATION;
#endif

	PEString::generatePathname(*m_pContext, filename, "Default", "GPUPrograms", PEString::s_buf, PEString::BUF_SIZE);
	FileReader f(PEString::s_buf);

	char *pFileContents = 0;
	PrimitiveTypes::UInt32 fileSize = 0;
	f.readIntoBuffer(pFileContents, fileSize);
	
	ID3D10Blob* pErrorBlob;
	HRESULT hr = D3DCompile(
		pFileContents, // p text
		fileSize, //text size
		PEString::s_buf, // name of the file
		pMacros, // macro
		D3D_COMPILE_STANDARD_FILE_INCLUDE, // includes handler
		name, // entrypoint
		version, // target profile
		dwShaderFlags,
		0, // ignored when compiling non .fx files
		&pShaderBuffer, // compilation output
		&pErrorBlob); // errors

	if (FAILED(hr))
	{
		if( pErrorBlob != NULL )
			PEINFOSTR( (char*)pErrorBlob->GetBufferPointer() );
	}
	if (pErrorBlob != NULL)
		pErrorBlob->Release();

	assert(SUCCEEDED(hr));

}

void Effect::loadSPEShader(const char *filename, const char *name, Handle &hres)
{
	assert(!"Not Implemented!");
#if 0
	Components::LuaEnvironment::Instance()->prepFunctionCall("CommandServer.loadSpeShader");

	lua_State *luaVM = Components::LuaEnvironment::Instance()->L;

	lua_newtable(luaVM);

	lua_pushstring(luaVM, "filename"); // 
	lua_pushstring(luaVM, m_spesFilename);
	lua_rawset(luaVM, -3); // set name

	lua_pushstring(luaVM, "text"); // 
	void *pdata = 0;
	Effect::readIntoBufferReplaceEOL(pdata, m_spesFilename);
	lua_pushstring(luaVM, (char *)(pdata));
	delete pdata;
	lua_rawset(luaVM, -3); // set vs text

	lua_pushstring(luaVM, "techName"); // 
	lua_pushstring(luaVM, m_techName);
	lua_rawset(luaVM, -3); // set name

	lua_pushstring(luaVM, "serverIP"); // 
	lua_pushstring(luaVM, "10.0.0.2");
	lua_rawset(luaVM, -3); // set name

	lua_pushstring(luaVM, "serverPort"); // 
	lua_pushstring(luaVM, "9999");
	lua_rawset(luaVM, -3); // set name

	Components::LuaEnvironment::Instance()->callPreparedFunction(1, 0, 0);

	// set all samplers to trilinear state
	// need to disable some for optimal performance (draw call doesn't usually use all 8, so some can be set to INVALID)
	for (int i = 0; i < NUM_GPU_SAMPLERS; i++)
		m_samplerStates[i] = CPU_SAMPLER_INDICES::INVALID;
#endif
}

void Effect::initSpeData(PrimitiveTypes::UInt32 stride, PrimitiveTypes::UInt32 numVals)
{
	if (m_speData.isValid())
		m_speData.release();

	m_speData = Handle("RAW_DATA", stride * numVals);
	memset(m_speData.getObject(), 0, m_speData.getSize());
}
void Effect::setSpeShaderSatus(PrimitiveTypes::Bool ready)
{
	lock();
	
	m_isReady = ready;
	
	unlock();

}

#endif
}; // namespace Components
}; // namespace PE
