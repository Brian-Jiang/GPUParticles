
// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

// Outer-Engine includes

// Inter-Engine includes
#include "PrimeEngine/Render/IRenderer.h"
#include "../../Geometry/MaterialCPU/MaterialCPU.h"
#include "../Texture/Texture.h"
#include "../Texture/GPUTextureManager.h"
#include "../Effect/EffectManager.h"

#include "PrimeEngine/Render/ShaderActions/SetPerMaterialConstantsShaderAction.h"
#include "PrimeEngine/Render/ShaderActions/SA_Bind_Resource.h"
// Sibling/Children includes
#include "GPUMaterial.h"

#include "../../Scene/DrawList.h"

namespace PE {

using namespace Components;

void GPUMaterial::createFromMaterialCPU(MaterialCPU &matcpu)
{
	m_diffuse = matcpu.m_diffuse;
	m_specular = matcpu.m_specular;
	m_emissive = matcpu.m_emissive;
	m_shininess = matcpu.m_shininess;
	
	StringOps::writeToString(matcpu.m_dbgName, m_dbgName, sizeof(m_dbgName));

	// some textures could be one texture (texture array)
	PrimitiveTypes::Bool isArray = false;
	PrimitiveTypes::UInt32 nTextures = 0;

	for (PrimitiveTypes::UInt32 iTex = 0; iTex < matcpu.m_textureFilenames.m_size; iTex++)
	{
		TextureFamily::TextureFamily_ family = matcpu.m_textureFamilies[iTex];
		if (family == TextureFamily::COLOR_MAP_ARRAY)
		{
			if (!isArray)
			{
				// first of array. inc counter
				isArray = true;
			}
			nTextures++;
		}
		else
		{
			isArray = false;
			nTextures++;
		}
	}

	m_textures.reset(nTextures);

	isArray = false;
	Array<char *> filenameArr(*m_pContext, m_arena);
	char arrPackage[256];

	filenameArr.reset(64);
	char filenames[64][256];

	PrimitiveTypes::UInt32 nArrayTextures = 0;

	for (PrimitiveTypes::UInt32 iTex = 0; iTex <  matcpu.m_textureFilenames.m_size; iTex++)
	{
		PEString &texFilename = matcpu.m_textureFilenames[iTex];
		PEString &texPackage = matcpu.m_texturePackages[iTex];

		ESamplerState customSamplerState = matcpu.m_customSamplerStates[iTex];

		TextureFamily::TextureFamily_ family = matcpu.m_textureFamilies[iTex];
		
		if (family == TextureFamily::COLOR_MAP_ARRAY)
		{
			isArray = true;
			StringOps::writeToString(texFilename.m_data.m_dataHandle.getObject<char>(), filenames[nArrayTextures], 255);
			filenameArr.add(filenames[nArrayTextures++]);
			StringOps::writeToString(texPackage.m_data.m_dataHandle.getObject<char>(), arrPackage, 255);
		}
		else
		{
			if (isArray)
			{
				// done going through names of array elements
				// need to add the array texture;
				Handle htgpu = GPUTextureManager::Instance()->createColorTextureArrayGPU(&(filenameArr[0]), nArrayTextures, arrPackage);
				m_textures.add(htgpu);
				isArray = false;
				nArrayTextures = 0;
				filenameArr.reset(64);
			}

			Handle htgpu;
			if (family == TextureFamily::COLOR_MAP)
			{
				htgpu = GPUTextureManager::Instance()->createColorTextureGPU(texFilename.m_data.getFirstPtr(), texPackage.m_data.getFirstPtr(), customSamplerState);
				m_xHasNrm_yHasSpec_zHasGlow_wHasDiff.m_w = 1;
			}

			else if (family == TextureFamily::COLOR_CUBE)
			{
				htgpu = GPUTextureManager::Instance()->createColorCubeTextureGPU(texFilename.m_data.getFirstPtr(), texPackage.m_data.getFirstPtr());
				m_xHasNrm_yHasSpec_zHasGlow_wHasDiff.m_w = 1;
			}
			else if (family == TextureFamily::NORMAL_MAP)
			{
				htgpu = GPUTextureManager::Instance()->createBumpTextureGPU(texFilename.m_data.getFirstPtr(), texPackage.m_data.getFirstPtr());
				m_xHasNrm_yHasSpec_zHasGlow_wHasDiff.m_x = 1;
			}
			else if (family == TextureFamily::SPECULAR_MAP)
			{
				htgpu = GPUTextureManager::Instance()->createSpecularTextureGPU(texFilename.m_data.getFirstPtr(), texPackage.m_data.getFirstPtr());
				m_xHasNrm_yHasSpec_zHasGlow_wHasDiff.m_y = 1;
			}
			else if (family == TextureFamily::GLOW_MAP)
			{
				htgpu = GPUTextureManager::Instance()->createGlowTextureGPU(texFilename.m_data.getFirstPtr(), texPackage.m_data.getFirstPtr());
				m_xHasNrm_yHasSpec_zHasGlow_wHasDiff.m_z = 1;
			}
			else
			{
				assert(!"Can't load texture without family specified");
			}
			m_textures.add(htgpu);
		}
		
	}
	if (isArray)
	{
		// need to add the array texture;
		Handle htgpu = GPUTextureManager::Instance()->createColorTextureArrayGPU(&filenameArr[0], nArrayTextures, arrPackage);
		m_textures.add(htgpu);
		isArray = false;
		nArrayTextures = 0;
		filenameArr.reset(64);
	}

	filenameArr.reset(0);
}

// for each texture, set its binding slot and bind it to pipeline based on its type
void GPUMaterial::createShaderActions(Components::DrawList *pDrawList)
{
	for (PrimitiveTypes::UInt32 itex = 0; itex < m_textures.m_size; itex++)
	{
		Handle hCurTex = m_textures[itex];
		TextureGPU &curTex = *hCurTex.getObject<TextureGPU>();

		Handle &hSV = pDrawList->nextShaderValue(sizeof(SA_Bind_Resource));
		// create object referenced by Handle in DrawList
		// this handle will be released on end of draw call, os no need to worry about realeasing this object
		SA_Bind_Resource *pSetTextureAction = new(hSV) SA_Bind_Resource(*m_pContext, m_arena);

		PE::EGpuResourceSlot bufferId = DIFFUSE_TEXTURE_2D_SAMPLER_SLOT;
		// todo add asserts for conflicting gpu slots
		// that is if two resources occupy same slot in same materials (Ex: 2 diffuse maps)
		if (curTex.m_family == TextureFamily::COLOR_MAP)
		{
			bufferId = DIFFUSE_TEXTURE_2D_SAMPLER_SLOT;
		}
		else if (curTex.m_family == TextureFamily::COLOR_CUBE)
		{
			bufferId = DIFFUSE_TEXTURE_CUBE_SLOT;
		}
		else if (curTex.m_family == TextureFamily::NORMAL_MAP)
		{
			bufferId = BUMPMAP_TEXTURE_2D_SAMPLER_SLOT;
		}
		else if (curTex.m_family == TextureFamily::SPECULAR_MAP)
		{
			bufferId = SPECULAR_TEXTURE_2D_SAMPLER_SLOT;
		}
		else if (curTex.m_family == TextureFamily::GLOW_MAP)
		{
			bufferId = GLOW_TEXTURE_2D_SAMPLER_SLOT;
		}
		else if (curTex.m_family == TextureFamily::COLOR_MAP_ARRAY)
		{
			bufferId = DIFFUSE_TEXTURE_2D_ARRAY_SLOT;
		}
		else
		{
			PEINFO("WARNING: Unknown texture family type can't be bound to pipeline!\n");
		}

		PEASSERT(curTex.m_samplerState != SamplerState_INVALID, "Invalid Sampler State For A Texture");

#if APIABSTRACTION_D3D9
#elif APIABSTRACTION_D3D11
		PEASSERT(curTex.m_pShaderResourceView != NULL, "texture shader resource not set");
#elif APIABSTRACTION_OGL
#elif PE_PLAT_IS_PSVITA
#endif
		pSetTextureAction->set(
			bufferId,
			curTex.m_samplerState,
#if APIABSTRACTION_D3D9
			curTex.m_pTexture,
#elif APIABSTRACTION_D3D11
			curTex.m_pShaderResourceView,
#elif APIABSTRACTION_OGL
			curTex.m_texture,
#elif PE_PLAT_IS_PSVITA
			curTex.m_texture,
#endif
			curTex.m_name
			);
		
	}
	
	// set constant buffer
	Handle &hSV = pDrawList->nextShaderValue(sizeof(SetPerMaterialConstantsShaderAction));
	// create object referenced by Handle in DrawList
	// this handle will be released on end of draw call
	SetPerMaterialConstantsShaderAction *pSV = new(hSV) SetPerMaterialConstantsShaderAction();

	pSV->m_data.m_diffuse = m_diffuse;
	pSV->m_data.gxyzVSpecular_w.asVector3Ref() = m_specular;
	pSV->m_data.gxyzVEmissive_wVShininess.asVector3Ref() = m_emissive;
	pSV->m_data.gxyzVEmissive_wVShininess.m_w = m_shininess;
	pSV->m_data.m_xHasNrm_yHasSpec_zHasGlow_wHasDiff = m_xHasNrm_yHasSpec_zHasGlow_wHasDiff;

}
void GPUMaterial::removeAllTexturesOfFamily(TextureFamily::TextureFamily_ family)
{
	for (PrimitiveTypes::UInt32 itex = 0; itex < m_textures.m_size; itex++)
	{
		TextureGPU &curTex = *m_textures[itex].getObject<TextureGPU>();
		if (curTex.m_family == family)
		{
			m_textures.remove(itex--);
		}
	}
}

}; // namespace PE
