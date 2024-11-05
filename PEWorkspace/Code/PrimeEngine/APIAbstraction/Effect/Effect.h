#ifndef __PYENGINE_2_0_EFFECT__
#define __PYENGINE_2_0_EFFECT__
// This class works with index buffers in GPU memory

// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

// Outer-Engine includes

// Inter-Engine includes

#if APIABSTRACTION_D3D9
//#include "../DirectX10/D3D10_Effect/D3D10_Effect.h"
#elif APIABSTRACTION_D3D11
//#include "../DirectX11/D3D11_Effect/D3D11_Effect.h"
#endif

#include "PrimeEngine/Render/IRenderer.h"
#include "PrimeEngine/Math/Matrix4x4.h"
#include "../Threading/Threading.h"
// Sibling/Children includes
#include "EffectEnums.h"
#include "PERasterizerState.h"
#include "PEAlphaBlendState.h"
#include "PEDepthStencilState.h"

#include "ExternalPerTechniqueData.h"

#include "SpeShader.h"

namespace PE {

struct PEAlphaBlendState;
struct PERasterizerState;
class VertexBufferGPU;
namespace Components {
struct Effect : public Component
{
	PE_DECLARE_CLASS(Effect);

	Effect(PE::GameContext &context, PE::MemoryArena arena, Handle hMyself);

	void loadFromCopy(Effect &cpy);

	void setPixelShader(Effect &cpy);

	void loadTechnique(const char *vsFilename, const char *vsName, 
		const char *gsFilename, const char *gsName,
		const char *psFilename, const char *psName,
		const char *csFilename, const char *csName,
		E_PERasterizerState rasterizerStateId,
		E_PEDepthStencilState depthStencilStateId,
		E_PEAlphaBlendState blendStateId,
		const char *techniqueName,
		bool reload = false);
	
	void loadTechniqueAsync(); // calls one of the sync functions in same or separate thread

#if APIABSTRACTION_D3D9
	void loadTechnique_D3D9_VS_PS(const char *vsFilename, const char *vsName, 
		const char *psFilename, const char *psName,
		const char *techniqueName,
		bool reload = false);
		
	void loadTechniqueSync_D3D9();


	//windows 8
	void loadShader_D3D9_WIN8(const char *filename, const char *name,
		D3D_SHADER_MACRO *pMacros,
		const char *version,
		ID3DBlob* &pShaderBuffer);

#elif APIABSTRACTION_D3D11
	
	void loadTechnique_D3D11(const char *vsFilename, const char *vsName, 
		const char *gsFilename, const char *gsName,
		const char *psFilename, const char *psName,
		const char *csFilename, const char *csName,
		const char *techniqueName,
		bool reload = false);

	void loadTechniqueSpeS(
		const char *spesFilename, const char *spesName,
		const char *techniqueName);
	
	void loadTechniqueSync_D3D11();

	void loadShader_D3D11(const char *filename, const char *name,
		D3D_SHADER_MACRO *pMacros,
		const char *version,
		ID3DBlob* &pShaderBuffer);

	void loadSPEShader(const char *filename, const char *name, Handle &hres);
	void setSpeShaderSatus(PrimitiveTypes::Bool ready);
	void initSpeData(PrimitiveTypes::UInt32 stride, PrimitiveTypes::UInt32 numVals);
#elif APIABSTRACTION_OGL
	void loadTechnique_GL(const char *vsFilename, const char *vsName, 
		const char *psFilename, const char *psName,
		const char *techniqueName,
		bool reload = false);

	void loadTechniqueSync_GL();
	
	bool glLoadShader(const char *filename, const char *name,
		/*D3DXMACRO *pMacros,*/
#       if APIABSTRACTION_IOS //ios uses original opengl API, not cg
            GLenum shaderType, GLuint &out_program); //todo:anything else here for ios?
#       else
            CGprofile cgProfile, CGprogram &out_program);
#       endif

    int processIncludes(const char *source, const char *curFile, const char*curFolderPath, PrimitiveTypes::UInt32 curPos, Array<PEString> &allIncludes, Array<PEString> &filenames, Array<int> &lineCounts);

#elif PE_PLAT_IS_PSVITA
	

#endif

#if APIABSTRACTION_D3D11
	static void setConstantBuffer(ID3D11Device *pDevice, ID3D11DeviceContext *pDeviceContex, ID3D11Buffer * pBuf, PrimitiveTypes::UInt32 slotId, void *pVal, PrimitiveTypes::UInt32 size);
#endif
	void setCurrent(VertexBufferGPU *pCurVertexBuffer);

	static void readIntoBufferReplaceEOL(PE::GameContext &context, char *&pdata, const char *filename);

	static void writeEffectFile(PE::GameContext &context, const char *filename, const char *ptext);

	PrimitiveTypes::Bool lock() { return m_changeMutex.lock(0); }

	void unlock() { m_changeMutex.unlock(); }

	//////////////////////////////////////////////////////////////////////////
	// Effect Lua Interface
	//////////////////////////////////////////////////////////////////////////
	//
	static void SetLuaFunctions(PE::Components::LuaEnvironment *pLuaEnv, lua_State *luaVM);
	//
	static int l_GetTechnique(lua_State *luaVM);
	//
	static int l_SetTechnique(lua_State *luaVM);
	//
	static int l_SetSpeShaderData(lua_State *luaVM);
	//
	static int l_SetSpeTechniqueReady(lua_State *luaVM);
	//
	//////////////////////////////////////////////////////////////////////////

#if APIABSTRACTION_D3D9
	IDirect3DVertexShader9 *m_VS;
	IDirect3DPixelShader9 *m_PS;
	IDirect3DVertexDeclaration9 *m_pInputLayout;
#elif APIABSTRACTION_D3D11
	ID3D11VertexShader *m_VS;
	ID3D11GeometryShader *m_GS;
	ID3D11PixelShader *m_PS;
	ID3D11ComputeShader *m_CS;
#elif APIABSTRACTION_OGL
	GLuint m_VS;
	GLuint m_PS;
	GLuint m_program;
    #if APIABSTRACTION_IOS
        GLuint m_cgVertexProgram, m_cgFragmentProgram, m_glslProgram;
    #else
        CGprogram m_cgVertexProgram, m_cgFragmentProgram;
    #endif
#elif PE_PLAT_IS_PSVITA
	
#endif

	Handle m_SpeS;

	PEAlphaBlendState * m_pBlendState;
	PERasterizerState * m_pRasterizerState;
    PEDepthStencilState *m_pDepthStencilState;

	EffectPSInputFamily::EffectPSInputFamily_ m_psInputFamily;

	EffectDrawOrder::EffectDrawOrder_ m_effectDrawOrder;
	char m_techName[256];
	
	char m_vsName[256];
	EPEVertexFormat m_vsVertexFormat;
	char m_vsFilename[256];
	
	char m_psName[256];
	char m_psFilename[256];
	

	char m_gsFilename[256];
	char m_gsName[256];
	
	char m_csFilename[256];
	char m_csName[256];

	char m_spesFilename[256];
	char m_spesName[256];
	Handle m_speData;
		

	PE::Threading::Mutex m_changeMutex;
	PrimitiveTypes::Bool m_isReady;

	// External per-technique data
	// Shader Action Data (per-technique binding to shader constants)
	// Vertex Buffer Daya (per-technique vertex attribute binding to shader inputs)
	ExternalPerTechniqueData m_externalPerTechniqueData;

	PE::MemoryArena m_arena; PE::GameContext *m_pContext;
};

}; // namespace Components
}; // namespace PE

#endif
