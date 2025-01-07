// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

// Outer-Engine includes

// Inter-Engine includes
#include "PrimeEngine/PrimitiveTypes/PrimitiveTypes.h"
#include "PrimeEngine/APIAbstraction/GPUBuffers/ResourceBufferGPU.h"
#include "PrimeEngine/APIAbstraction/Effect/Effect.h"

// Sibling/Children includes
#include "SA_Bind_Resource.h"
namespace PE {
using namespace Components;
SA_Bind_Resource::SA_Bind_Resource(PE::GameContext &context, PE::MemoryArena arena)
{
	m_arena = arena; m_pContext = &context;
	m_samplerState = SamplerState_INVALID;
}

SA_Bind_Resource::SA_Bind_Resource(
	PE::GameContext &context, PE::MemoryArena arena,
	EGpuResourceSlot bufferId,
	ESamplerState samplerState,
	API_CHOOSE_DX11_DX9_OGL_PSVITA_NO_PAREN(ID3D11ShaderResourceView* pShaderResource, IDirect3DTexture9* pTextureResource, GLuint texture, void texture),
	const char *dbgStr/* = NULL*/
)
: ShaderAction()
{
	m_arena = arena; m_pContext = &context;
	#if PE_PLAT_IS_PSVITA
		set(bufferId, samplerState, texture, dbgStr);
	#elif APIABSTRACTION_D3D9
		set(bufferId, samplerState, pTextureResource, dbgStr);
	#elif APIABSTRACTION_D3D11
		set(bufferId, samplerState, pShaderResource, dbgStr);
	#elif APIABSTRACTION_OGL
		set(bufferId, samplerState, texture, dbgStr);
	#endif
}

// Set the SRV binding
void SA_Bind_Resource::set(
	EGpuResourceSlot bufferId,
	ESamplerState samplerState,
	API_CHOOSE_DX11_DX9_OGL_PSVITA_NO_PAREN(ID3D11ShaderResourceView* pShaderResource, IDirect3DTexture9* pTextureResource, GLuint texture, void texture),
	const char *dbgStr/* = NULL*/
	)
{
	m_dbgStr = dbgStr;
	m_samplerState = samplerState;

	m_bufferId = bufferId;
#if PE_PLAT_IS_PSVITA
	m_texture = texture;
#elif APIABSTRACTION_D3D9
	m_pTextureResource = pTextureResource;
#elif APIABSTRACTION_D3D11
	m_pShaderResource = pShaderResource;
#elif APIABSTRACTION_OGL
	m_texture = texture;
#endif
}

// Set the UAV binding
#if APIABSTRACTION_D3D11
void SA_Bind_Resource::set(
	EGpuResourceSlot bufferId,
	ID3D11UnorderedAccessView* pShaderResource,
	ID3D11Buffer *pCopyBackBuffer,
	void *pCopyBackMem,
	int memSize,
	const char *dbgStr/* = NULL*/
	)
{
	m_dbgStr = dbgStr;
	m_samplerState = SamplerState_NotNeeded;
	m_bufferId = bufferId;
	m_pShaderUAV = pShaderResource;
	m_pCopyBackBuffer = pCopyBackBuffer;
	m_pCompyBackMem = pCopyBackMem;
	m_memSize = memSize;
}
#endif

void SA_Bind_Resource::bindToPipeline(Effect *pCurEffect /* = NULL*/)
{
	assert(m_samplerState != SamplerState_INVALID);
#if PE_PLAT_IS_PSVITA
#elif APIABSTRACTION_D3D9
		D3D9Renderer *pD3D9Renderer = static_cast<D3D9Renderer *>(m_pContext->getGPUScreen());
		LPDIRECT3DDEVICE9 pDevice = pD3D9Renderer->m_pD3D9Device;
		
		pDevice->SetTexture(m_bufferId, m_pTextureResource);
		/*
		pDevice->VSSetShaderResources(m_bufferId, 1, &pRV);
		pDevice->GSSetShaderResources(m_bufferId, 1, &pRV);
		pDevice->PSSetShaderResources(m_bufferId, 1, &pRV);
		*/
		if (m_samplerState != SamplerState_INVALID)
		{
			SamplerState &ss = SamplerStateManager::getInstance()->getSamplerState(m_samplerState);

			pDevice->SetSamplerState(m_bufferId, D3DSAMP_ADDRESSU, ss.val_D3DSAMP_ADDRESSU);
			pDevice->SetSamplerState(m_bufferId, D3DSAMP_ADDRESSV, ss.val_D3DSAMP_ADDRESSV);
			pDevice->SetSamplerState(m_bufferId, D3DSAMP_ADDRESSW, ss.val_D3DSAMP_ADDRESSW);

			//val_D3DSAMP_BORDERCOLOR,

			pDevice->SetSamplerState(m_bufferId, D3DSAMP_MAGFILTER, ss.val_D3DSAMP_MAGFILTER);
			pDevice->SetSamplerState(m_bufferId, D3DSAMP_MINFILTER, ss.val_D3DSAMP_MINFILTER);
			pDevice->SetSamplerState(m_bufferId, D3DSAMP_MIPFILTER, ss.val_D3DSAMP_MIPFILTER);

			//val_D3DSAMP_MIPMAPLODBIAS,
			//val_D3DSAMP_MAXMIPLEVEL,
			//val_D3DSAMP_MAXANISOTROPY,
			//val_D3DSAMP_SRGBTEXTURE,
			//val_D3DSAMP_ELEMENTINDEX,
			//val_D3DSAMP_DMAPOFFSET
		}

    #elif APIABSTRACTION_D3D11

		D3D11Renderer *pD3D11Renderer = static_cast<D3D11Renderer *>(m_pContext->getGPUScreen());
		ID3D11Device *pDevice = pD3D11Renderer->m_pD3DDevice;
		ID3D11DeviceContext *pDeviceContext = pD3D11Renderer->m_pD3DContext;

		if (m_bufferId < GpuResourceSlot_FirstUAV)
		{
			pDeviceContext->VSSetShaderResources(m_bufferId, 1, &m_pShaderResource);
			pDeviceContext->GSSetShaderResources(m_bufferId, 1, &m_pShaderResource);
			pDeviceContext->PSSetShaderResources(m_bufferId, 1, &m_pShaderResource);
			pDeviceContext->CSSetShaderResources(m_bufferId, 1, &m_pShaderResource);
#if DEBUG_DRAW_CALLS
			PEINFO("SA_Bind_Resource::bindToPipeline(): Binding Resource View %p (%s) to slot t %d\n", m_pShaderResource, m_dbgStr, m_bufferId);
#endif
			if (m_samplerState != SamplerState_INVALID && m_samplerState != SamplerState_NotNeeded)
			{
#if DEBUG_DRAW_CALLS
				PEINFO("SA_Bind_Resource::bindToPipeline(): Binding Sampler %d to slot s %d \n", int(m_samplerState), m_bufferId);
#endif
				SamplerState &ss = SamplerStateManager::getInstance()->getSamplerState(m_samplerState);
				pDeviceContext->VSSetSamplers(m_bufferId, 1, &ss.m_pd3dSamplerState);
				pDeviceContext->PSSetSamplers(m_bufferId, 1, &ss.m_pd3dSamplerState);
			}
		}
		else
		{
			UINT offset = -1;// keeps offset the same
#if DEBUG_DRAW_CALLS
			PEINFO("SA_Bind_Resource::bindToPipeline(): Binding RW Resource %p (%s) to slot u %d\n", m_pShaderUAV, m_dbgStr, m_bufferId-GpuResourceSlot_FirstUAV);
#endif
			pDeviceContext->CSSetUnorderedAccessViews(m_bufferId-GpuResourceSlot_FirstUAV, 1, &m_pShaderUAV, &offset);
		}
    #elif APIABSTRACTION_OGL
		ExternalPerTechniqueData &data = pCurEffect->m_externalPerTechniqueData;
	
        #if APIABSTRACTION_IOS 
            glActiveTexture(GL_TEXTURE0 + m_bufferId);
            glBindTexture(GL_TEXTURE_2D, m_texture);
            
            if(data.m_BindResourceToGPUPipelineAction_cgparameter.m_samplerParameters[m_bufferId] != -1)
                glUniform1i(data.m_BindResourceToGPUPipelineAction_cgparameter.m_samplerParameters[m_bufferId], m_bufferId);
		#else
			//artem: note, this MIGHT need to be called after programs are bound. idk why, but in the cg sample they do it after binding program. also in samples they set other non-sampler uniform params before binding program.
            // CGGL references a value in shader in CGparameter struct. Even if resource has the same name (g_diffuse) it could be bound to different registers in differetn shaders
            // tahst why CGparameters are stored per effect
            CGparameter f_gXXXSampler = data.m_BindResourceToGPUPipelineAction_cgparameter.m_samplerParameters[m_bufferId];

			//printf("cgIsParameter(f_gXXXSampler) = %d\n", cgIsParameter(f_gXXXSampler));


			/*
			{
				//cgGLGetTextureEnum doesnt seem to work on ps3.. tried it with params that return true for cgIsParameter()
				GLenum samplerstextureUnit = cgGLGetTextureEnum(f_gXXXSampler);
				IRenderer::checkForErrors("cgGLGetTextureEnum()\n");

				printf("Sampler's texture Unit: val = %d = GL_TEXTURE%d \n", samplerstextureUnit, samplerstextureUnit - GL_TEXTURE0);
				printf("Setting client state current texture unit to val = %d = GL_TEXTURE%d\n", samplerstextureUnit, samplerstextureUnit - GL_TEXTURE0);
				glClientActiveTexture(samplerstextureUnit);
			}
			*/
			
			
			/*
			//this functionality seems to be missing for ps3
			{
				GLint boundTexture = 0;
				glGetIntegerv(GL_TEXTURE_2D, &boundTexture);
				printf("bound texture id = %d\n", boundTexture);
			}
			*/

			//printf("Setting samplers texture object to texture id = %d\n", m_texture);
			if (f_gXXXSampler) // parameter could be invalid if shader doesnt use the texture
			{
				cgGLSetTextureParameter(f_gXXXSampler, m_texture);
				IRenderer::checkForErrors("cgGLSetTextureParameter()\n");

				//printf("Enabling sampler parameter\n");
				cgGLEnableTextureParameter(f_gXXXSampler); //must be called after cgGLSetTextureParameter()
				IRenderer::checkForErrors("cgGLEnableTextureParameter()\n");
			}

			/*
			{
				GLint boundTexture = 0;
				glGetIntegerv(GL_TEXTURE_2D, &boundTexture);
				printf("bound texture id = %d\n", boundTexture);
			}
			*/

			
        #endif
	
            //cgGLUpdateProgramParameters(data.m_cgVertexProgram);
            //cgGLUpdateProgramParameters(data.m_cgFragmentProgram);
        
		// no need to set sampler state here since it lives with the texture
    #endif // OGL
}

void SA_Bind_Resource::unbindFromPipeline(Components::Effect *pCurEffect /* = NULL*/)
{

	#if PE_PLAT_IS_PSVITA
	#elif APIABSTRACTION_D3D9
		D3D9Renderer *pD3D9Renderer = static_cast<D3D9Renderer *>(m_pContext->getGPUScreen());
		LPDIRECT3DDEVICE9 pDevice = pD3D9Renderer->m_pD3D9Device;
		
		pDevice->SetTexture(m_bufferId, NULL);
    #elif APIABSTRACTION_D3D11

		D3D11Renderer *pD3D11Renderer = static_cast<D3D11Renderer *>(pCurEffect->m_pContext->getGPUScreen());
		ID3D11Device *pDevice = pD3D11Renderer->m_pD3DDevice;
		ID3D11DeviceContext *pDeviceContext = pD3D11Renderer->m_pD3DContext;

		if (m_bufferId < GpuResourceSlot_FirstUAV)
		{
			
			ID3D11ShaderResourceView *pV = NULL;
			pDeviceContext->VSSetShaderResources(m_bufferId, 1, &pV);
			pDeviceContext->GSSetShaderResources(m_bufferId, 1, &pV);
			pDeviceContext->PSSetShaderResources(m_bufferId, 1, &pV);
			pDeviceContext->CSSetShaderResources(m_bufferId, 1, &pV);
			
			//if (m_samplerState != SamplerState_INVALID && m_samplerState != SamplerState_NotNeeded)
			//do we need to unset samplers too?
			//{
			//	pDeviceContext->VSSetSamplers(m_bufferId, 1, &ss.m_pd3dSamplerState);
			//	pDeviceContext->PSSetSamplers(m_bufferId, 1, &ss.m_pd3dSamplerState);
			//}	
		}
		else
		{
			ID3D11UnorderedAccessView *pUAV = NULL;
			UINT offset = -1;// keeps offset the same
			pDeviceContext->CSSetUnorderedAccessViews(m_bufferId-GpuResourceSlot_FirstUAV, 1, &pUAV, &offset);

			if (m_pCopyBackBuffer)
			{
				
				// copy data back: note this is extremely inefficient, we are doing this right after submitting draw call whihc means we will stall cpu until gpu is done
				// with current draw call. this is for debug only.
				ID3D11Resource *pResource;
				m_pShaderUAV->GetResource(&pResource);

#if DEBUG_DRAW_CALLS
				PEINFO("SA_Bind_Resource::unbindFromPipeline(): Copying RW Resource %p from view %p into staged resource %p then into memory %p size %d\n", pResource, m_pShaderUAV, m_pCopyBackBuffer, m_pCompyBackMem, m_memSize);
#endif
				pD3D11Renderer->m_pD3DContext->CopyResource(m_pCopyBackBuffer, pResource);
				D3D11_MAPPED_SUBRESOURCE mappedResource;
				HRESULT hr;

				hr = pD3D11Renderer->m_pD3DContext->Map(
					m_pCopyBackBuffer, // resource
					0, // subresource
					D3D11_MAP_READ, 
					0,//D3D11_MAP_FLAG_DO_NOT_WAIT, //0, 
					&mappedResource);

				PEASSERT(SUCCEEDED(hr), "Unsuccessful Map\n");
				memcpy(m_pCompyBackMem, mappedResource.pData, m_memSize);
				pD3D11Renderer->m_pD3DContext->Unmap(m_pCopyBackBuffer, 0);
			}
		}
		
	#elif APIABSTRACTION_OGL
		ExternalPerTechniqueData &data = pCurEffect->m_externalPerTechniqueData;
		#if !APIABSTRACTION_IOS 
			CGparameter f_gXXXSampler = data.m_BindResourceToGPUPipelineAction_cgparameter.m_samplerParameters[m_bufferId];
			
			if (f_gXXXSampler)
			{
				cgGLSetTextureParameter(f_gXXXSampler, 0);
				cgGLDisableTextureParameter(f_gXXXSampler);
			}

			// the code above should work bu it doesnt seem to make difference. texturing is still enabled on the sampler and it uses previously set texture
			//long way of doing it of setting the texture to 0 and reenabling gets better results because at least when texture i sampled, it returns constant white (or black)
// 			{
// 				printf("Disabling Sampler Parameter\n");
// 				cgGLDisableTextureParameter(f_gXXXSampler);
// 				IRenderer::checkForErrors("cgGLDisableTextureParameter()\n");
// 
// 				printf("Setting samplers texture object to texture id = 0\n");
// 				cgGLSetTextureParameter(f_gXXXSampler, 0);
// 				IRenderer::checkForErrors("cgGLEnableTextureParameter()\n");
// 
// 				printf("Enabling sampler parameter (to bind the 0 texture to its texture unit)\n");
// 				cgGLEnableTextureParameter(f_gXXXSampler); //must be called after cgGLSetTextureParameter()
// 				IRenderer::checkForErrors("cgGLEnableTextureParameter()\n");
// 			}
			
		#endif
		IRenderer::checkForErrors("unbinding shader parameter\n");
	#endif

} // no action needed, other texture can override this gpu slot

void SA_Bind_Resource::releaseData()
{} // no data held


#if !APIABSTRACTION_D3D9 && !APIABSTRACTION_D3D11 && !PE_PLAT_IS_PSVITA
static void FindSamplerBinding(Components::Effect *pEffect, SA_Bind_Resource::ApiBindingType &resBinding, const char *name)
{
#if APIABSTRACTION_IOS 
	resBinding = glGetUniformLocation(pEffect->m_glslProgram, name);
	NSLog(@"%s uniform location: %d", name, resBinding);
#else
	resBinding = cgGetNamedParameter(pEffect->m_cgFragmentProgram, name);
	IRenderer::checkForErrors("could not get sampler parameter\n");
#endif
}

void SA_Bind_Resource::PerEffectBindIds::initialize(Effect *pEffect)
{
#if APIABSTRACTION_IOS 

	FindSamplerBinding(pEffect,  m_samplerParameters[DIFFUSE_TEXTURE_2D_SAMPLER_SLOT], "gDiffuseMapSampler");
	FindSamplerBinding(pEffect,  m_samplerParameters[GLOW_TEXTURE_2D_SAMPLER_SLOT], "gGlowMapSampler");
	FindSamplerBinding(pEffect,  m_samplerParameters[BUMPMAP_TEXTURE_2D_SAMPLER_SLOT], "gBumpMapSampler");
	FindSamplerBinding(pEffect,  m_samplerParameters[SPECULAR_TEXTURE_2D_SAMPLER_SLOT], "gSpecularMapSampler");
	FindSamplerBinding(pEffect,  m_samplerParameters[SHADOWMAP_TEXTURE_2D_SAMPLER_SLOT], "gShadowMapSampler");
	FindSamplerBinding(pEffect,  m_samplerParameters[DIFFUSE_BLUR_TEXTURE_2D_SAMPLER_SLOT], "gBlurredGlowMapSampler");

#else
	//note from CG docs:
	//Note that when your
	//	application makes OpenGL calls to initialize the texture environment for a
	//	given sampler, it is important to remember to set the active texture unit to
	//	that associated with the sampler before doing so. The sampler’s texture unit
	//	can be retrieved by calling cgGLGetTextureEnum();

	memset(m_samplerParameters, 0, sizeof(m_samplerParameters));
	memset(m_samplerParameterNames, 0, sizeof(m_samplerParameterNames));

	FindSamplerBinding(pEffect,  m_samplerParameters[DIFFUSE_TEXTURE_2D_SAMPLER_SLOT], "gDiffuseMapSampler");
	m_samplerParameterNames[DIFFUSE_TEXTURE_2D_SAMPLER_SLOT] = "gDiffuseMapSampler";

	FindSamplerBinding(pEffect,  m_samplerParameters[GLOW_TEXTURE_2D_SAMPLER_SLOT], "gGlowMapSampler");
	m_samplerParameterNames[GLOW_TEXTURE_2D_SAMPLER_SLOT] = "gGlowMapSampler";

	FindSamplerBinding(pEffect,  m_samplerParameters[BUMPMAP_TEXTURE_2D_SAMPLER_SLOT], "gBumpMapSampler");
	m_samplerParameterNames[BUMPMAP_TEXTURE_2D_SAMPLER_SLOT] = "gBumpMapSampler";

	FindSamplerBinding(pEffect,  m_samplerParameters[SPECULAR_TEXTURE_2D_SAMPLER_SLOT], "gSpecularMapSampler");
	m_samplerParameterNames[SPECULAR_TEXTURE_2D_SAMPLER_SLOT] ="gSpecularMapSampler";

	FindSamplerBinding(pEffect,  m_samplerParameters[SHADOWMAP_TEXTURE_2D_SAMPLER_SLOT], "gShadowMapSampler");
	m_samplerParameterNames[SHADOWMAP_TEXTURE_2D_SAMPLER_SLOT] ="gShadowMapSampler";

	FindSamplerBinding(pEffect,  m_samplerParameters[DIFFUSE_BLUR_TEXTURE_2D_SAMPLER_SLOT], "gBlurredGlowMapSampler");
	m_samplerParameterNames[DIFFUSE_BLUR_TEXTURE_2D_SAMPLER_SLOT] ="gBlurredGlowMapSampler";
#endif
}
#endif


}; // namespace PE
