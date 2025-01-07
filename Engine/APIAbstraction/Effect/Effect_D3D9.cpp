#define NOMINMAX
#include "Effect.h"

#include "EffectManager.h"

#include "PrimeEngine/FileSystem/FileWriter.h"
#include "../../Lua/LuaEnvironment.h"
#include "../../Lua/EventGlue/EventDataCreators.h"
#include "PrimeEngine/Utils/ErrorHandling.h"

#include "PrimeEngine/Game/Common/GameContext.h"

#include "PrimeEngine/APIAbstraction/Texture/SamplerState.h"
namespace PE {

namespace Components {


#if APIABSTRACTION_D3D9

void Effect::loadTechnique_D3D9_VS_PS(const char *vsFilename, const char *vsName, 
    const char *psFilename, const char *psName,
    const char *techniqueName,
    bool reload)
{
	lock();

	m_isReady = false;

	StringOps::writeToString(techniqueName, m_techName, 256);

	StringOps::writeToString(vsName, m_vsName, 256);
	StringOps::writeToString(vsFilename, m_vsFilename, 256);

	StringOps::writeToString(psName, m_psName, 256);
	StringOps::writeToString(psFilename, m_psFilename, 256);

	StringOps::writeToString("", m_gsName, 256);
	StringOps::writeToString("", m_gsFilename, 256);

	StringOps::writeToString("", m_csName, 256);
	StringOps::writeToString("", m_csFilename, 256);

	StringOps::writeToString("", m_spesFilename, 256);
	StringOps::writeToString("", m_spesName, 256);

	unlock();

	loadTechniqueAsync();
}

// called in a separate thread
void Effect::loadTechniqueSync_D3D9()
{
	//assert(!"not implemented!");
	lock();

	ID3DBlob *pVertexShaderBuffer = NULL;
	ID3DBlob *pPixelShaderBuffer = NULL;

	IDirect3DVertexShader9 *pVertexShader = NULL;
	IDirect3DPixelShader9 *pPixelShader = NULL;
	
	if (StringOps::length(m_spesFilename))
	{
		return;
	}
	if (StringOps::length(m_gsFilename))
	{
		return;
	}
	if (StringOps::length(m_csFilename))
	{
		return;
	}

	D3D_SHADER_MACRO Shader_Macros[3] = { {"HLSL_SEPARATE_LOAD", "1"} , {"APIABSTRACTION_D3D9", "1"} , {0, 0}};

	if (StringOps::length(m_vsFilename))
	{
		loadShader_D3D9_WIN8(m_vsFilename, m_vsName, Shader_Macros,
			"vs_3_0", pVertexShaderBuffer);
	}

	D3D9Renderer *pD3D9Renderer = static_cast<D3D9Renderer *>(m_pContext->getGPUScreen());
	LPDIRECT3DDEVICE9 pDevice = pD3D9Renderer->m_pD3D9Device;
		
	if (StringOps::length(m_psFilename))
	{
		int existingIndex = EffectManager::Instance()->m_pixelShaders.findIndex(m_psName);
				
		/* todo: don't compile pixel shaders
		if (existingIndex != -1)
		{
			pPixelShader = EffectManager::Instance()->m_pixelShaders.m_pairs[existingIndex];
		}
		else
		*/
		{

			loadShader_D3D9_WIN8(m_psFilename, m_psName, Shader_Macros,
				"ps_3_0", pPixelShaderBuffer);

			if (pPixelShaderBuffer)
			{
				HRESULT hr = pDevice->CreatePixelShader((DWORD*)(pPixelShaderBuffer->GetBufferPointer()),
					&pPixelShader);
				assert(SUCCEEDED(hr));

				pPixelShaderBuffer->Release();
				EffectManager::Instance()->m_pixelShaders.add(m_psName, pPixelShader);
			}
		}
	}

	
	LPDIRECT3DVERTEXDECLARATION9 pVertexDeclaration = NULL;
	
	HRESULT hr;

	if (pVertexShaderBuffer)
	{
		hr = pDevice->CreateVertexShader((DWORD*)(pVertexShaderBuffer->GetBufferPointer()),
			&pVertexShader);

		PEASSERT(hr != D3DERR_INVALIDCALL, "Failed To Create Vertex Shader (it did compile though)\nError hr = D3DERR_INVALIDCALL");
		
		if (hr == E_OUTOFMEMORY)
		{

			MessageBoxA(0, "Failed To Create Vertex Shader (it did compile though) \
						   \nError hr = E_OUTOFMEMORY", "PyEngine Error", 0);
		}
		if (hr == D3DERR_OUTOFVIDEOMEMORY)
		{
			MessageBoxA(0, "Failed To Create Vertex Shader (it did compile though) \
						   \nError hr = D3DERR_OUTOFVIDEOMEMORY", "PyEngine Error", 0);
		}
		pVertexShaderBuffer->Release();
	
		if (FAILED(hr))
			return;
	}

	m_pInputLayout = pVertexDeclaration;
	m_VS = pVertexShader;
	m_PS = pPixelShader;
	
	m_externalPerTechniqueData.initVertexAttributeBindings(this);
	m_externalPerTechniqueData.init(this);

	m_isReady = true;

	unlock();
}

void Effect::loadShader_D3D9_WIN8(const char *filename, const char *name,
						D3D_SHADER_MACRO *pMacros,
						const char *version,
						ID3DBlob* &pShaderBuffer)
{
	PEString::generatePathname(*m_pContext, filename, "Default", "GPUPrograms", PEString::s_buf, PEString::BUF_SIZE);
	
	FileReader f(PEString::s_buf);

	char *pFileContents = 0;
	PrimitiveTypes::UInt32 fileSize = 0;
	f.readIntoBuffer(pFileContents, fileSize);
	
	PEINFO("PE: PROGRESS: loading shader: %s from %s\n", name, filename);

	DWORD dwShaderFlags = 0;// D3DXSHADER_USE_LEGACY_D3DX9_31_DLL; // enable this flag to use old compiler that will not try to parse DX10 shader code

#if defined( DEBUG ) || defined( _DEBUG )
	// Set the D3D10_SHADER_DEBUG flag to embed debug information in the shaders.
	// Setting this flag improves the shader debugging experience, but still allows 
	// the shaders to be optimized and to run exactly the way they will run in 
	// the release configuration of this program.
	dwShaderFlags |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION | D3DCOMPILE_PACK_MATRIX_COLUMN_MAJOR;
#endif

	ID3DBlob *pErrorBlob = NULL;
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
		PEINFO("D3DXCompileShaderFromFile() Failed\n");
		if( pErrorBlob != NULL )
			PEINFOSTR( (char*)pErrorBlob->GetBufferPointer() );
		MessageBoxA(0, "Shader Failed To Compile\nRefer to output window for more detail", "PyEngine Error", 0);
	}
	else
	{
		PEINFO("D3DXCompileShaderFromFile() Succeeded\n");
	}

	free(pFileContents);

	/*D3DX reference how tio find out register numbers for variables
	D3DXHANDLE h = pCTable->GetConstantByName(NULL, "gIGameTime");
	D3DXCONSTANT_DESC descs[10];
	UINT size = 10;
	pCTable->GetConstantDesc(h, descs, &size);
	*/
	if (pErrorBlob != NULL)
		pErrorBlob->Release();

	assert(SUCCEEDED(hr));
}


#endif
}; // namespace Components
}; // namespace PE
