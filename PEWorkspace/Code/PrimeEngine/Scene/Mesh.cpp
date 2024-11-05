#define NOMINMAX
// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

#include "Mesh.h"
// Outer-Engine includes

// Inter-Engine includes
#include "PrimeEngine/FileSystem/FileReader.h"
#include "PrimeEngine/APIAbstraction/GPUMaterial/GPUMaterialSet.h"
#include "PrimeEngine/PrimitiveTypes/PrimitiveTypes.h"
#include "PrimeEngine/APIAbstraction/Texture/Texture.h"
#include "PrimeEngine/APIAbstraction/Effect/EffectManager.h"
#include "PrimeEngine/APIAbstraction/GPUBuffers/VertexBufferGPUManager.h"
#include "PrimeEngine/../../GlobalConfig/GlobalConfig.h"
#include "Light.h"

// Sibling/Children includes

#include "MeshInstance.h"
#include "SceneNode.h"
#include "DrawList.h"
#include "SH_DRAW.h"
#include "../Lua/LuaEnvironment.h"

namespace PE {
namespace Components{

PE_IMPLEMENT_CLASS1(Mesh, Component);

Mesh::Mesh(PE::GameContext &context, PE::MemoryArena arena, Handle hMyself)
	: Component(context, arena, hMyself), m_additionalShaderValues(context, arena, 8), m_effects(context, arena)
	, m_zOnlyEffects(context, arena), m_instanceEffects(context, arena)
	, m_instances(context, arena, 16)
	, m_lods(context, arena, 4)
{
	m_processShowEvt = true;
    m_performBoundingVolumeCulling = false;
    m_isParticleEmitter = false;
}


void Mesh::addComponent(Handle hComponent, int *pAllowedEvents /* = NULL */)
{
	if (hComponent.getObject<Component>()->isInstanceOf<MeshInstance>())
	{
		m_instances.add(hComponent);
	}

	Component::addComponent(hComponent, pAllowedEvents);
}

void Mesh::removeComponent(int index)
{
	Handle hComponent = m_components[index];
	if (hComponent.getObject<Component>()->isInstanceOf<MeshInstance>())
	{
		m_instances.remove(m_instances.indexOf(hComponent));
	}
	
	Component::removeComponent(index);
}


void Mesh::addDefaultComponents()
{
	Component::addDefaultComponents();

	addComponent(SingleHandler_DRAW::GetInstanceHandle());
	addComponent(PESSEH_CHANGE_TO_DEBUG_SHADER::GetInstanceHandle());
	addComponent(PESSEH_POP_SHADERS::GetInstanceHandle());
}

void chooseEffects(MaterialCPU &curMatCpu, bool hasBlendShapes, EPEVertexFormat format,
	PEStaticVector<Handle, 4> &effects, PEStaticVector<Handle, 4> &shadowMapEffects, PEStaticVector<Handle, 4> &instanceEffects)
{
	PrimitiveTypes::Bool hasColorMap = false;
	PrimitiveTypes::Bool hasColorCube = false;
	PrimitiveTypes::Bool hasBumpMap = false;
	PrimitiveTypes::Bool hasSpecularMap = false;
	PrimitiveTypes::Bool hasGlowMap = false;

	for (PrimitiveTypes::UInt32 itex = 0; itex < curMatCpu.m_textureFamilies.m_size; itex++)
	{
		switch (curMatCpu.m_textureFamilies[itex])
		{
		case TextureFamily::COLOR_MAP: hasColorMap = true; break;
		case TextureFamily::COLOR_CUBE:hasColorCube = true; break;
		case TextureFamily::NORMAL_MAP: hasBumpMap = true; break;
		case TextureFamily::SPECULAR_MAP: hasSpecularMap = true; break;
		case TextureFamily::GLOW_MAP: hasGlowMap = true; break;
		default: assert(0); // error
		};
	}

	if (curMatCpu.m_preferredTechName[0])
	{
		char preferredTechName[64];
		preferredTechName[0] = '\0';

		sprintf(preferredTechName, "%s_%s_Tech", toString(format), curMatCpu.m_preferredTechName);

		PEINFO("Choosing technique %s from preferred technique name in material\n", preferredTechName);

		Handle h = EffectManager::Instance()->getEffectHandle(preferredTechName);
		PEASSERT(h.isValid(), "Did not find preferred effect %s\n", preferredTechName);
		effects.add(h);

		sprintf(preferredTechName, "%s_ZOnly_Tech", toString(format), curMatCpu.m_preferredTechName);

		PEINFO("Choosing zonly technique %s from preferred technique name in material\n", preferredTechName);

		h = EffectManager::Instance()->getEffectHandle(preferredTechName);
		PEASSERT(h.isValid(), "Did not find preferred effect %s\n", preferredTechName);

		shadowMapEffects.add(h);

		// todo: add check if supports instancing
		//instanceEffects;// = EffectManager::Instance()->getEffectHandle("DetailedMesh_Shadowed_A_Glow_Tech");

		return;
	}

	// choose effect for each material
	if (!hasBlendShapes)
	{
		if (format == PEVertexFormat_DetailedMesh || format == PEVertexFormat_DetailedSkin)
		{
			PEASSERT(hasBumpMap, "DetailedMesh has to have a normal map. the geometry should have been reduced to StdMesh before if normal map is not present");
			//todo : have glow and no glow versions
			if (format == PEVertexFormat_DetailedMesh)
			{
				effects.add(EffectManager::Instance()->getEffectHandle("DetailedMesh_Shadowed_A_Glow_Tech"));
				shadowMapEffects.add(EffectManager::Instance()->getEffectHandle("DetailedMesh_ZOnly_Tech"));
				//instanceEffects;// = EffectManager::Instance()->getEffectHandle("DetailedMesh_Shadowed_A_Glow_Tech");
			}
			else
			{
				effects.add(EffectManager::Instance()->getEffectHandle("DetailedSkin_Shadowed_A_Glow_Tech"));
				shadowMapEffects.add(EffectManager::Instance()->getEffectHandle("DetailedSkin_ZOnly_Tech"));

#if PE_API_IS_D3D11
				//instanceEffects.add(EffectManager::Instance()->getEffectHandle("DetailedSkin_Instanced_CS_Tech"));
				//instanceEffects.add(EffectManager::Instance()->getEffectHandle("DetailedSkin_Reduce_Instanced_CS_Tech"));
				//instanceEffects.add(EffectManager::Instance()->getEffectHandle("DetailedSkin_Shadowed_Instanced_Tech"));
#endif
			}
		}
		else if (format == PEVertexFormat_StdMesh || format == PEVertexFormat_StdSkin)
		{
			if (hasColorMap && hasGlowMap)
			{
				// MeshPosTexNormBAlphaGlow
				if (format == PEVertexFormat_StdMesh)
				{
					effects.add(EffectManager::Instance()->getEffectHandle("StdMesh_Shadowed_BAlphaGlow_Tech"));
					shadowMapEffects.add(EffectManager::Instance()->getEffectHandle("ReducedMesh_BAlphaGlow_ZOnly_Tech"));
					//instanceEffects;//todo: create instance effects = EffectManager::Instance()->getEffectHandle("DetailedMesh_Shadowed_A_Glow_Tech");
				}
				else
				{
					PEASSERT(false, "We dont support this combination");
				}
			}
			else
			{
				if (format == PEVertexFormat_StdMesh)
				{
					effects.add(EffectManager::Instance()->getEffectHandle("StdMesh_Shadowed_A_0_Tech"));
					shadowMapEffects.add(EffectManager::Instance()->getEffectHandle("StdMesh_ZOnly_Tech"));
					//instanceEffects;//todo: create instance effects = EffectManager::Instance()->getEffectHandle("DetailedMesh_Shadowed_A_Glow_Tech");
				}
				else
				{
					PEASSERT(false, "We dont have a technique for std skin");
				}
			}
		}
		else if (format == PEVertexFormat_ColoredMinimalMesh)
		{
			effects.add(EffectManager::Instance()->getEffectHandle("ColoredMinimalMesh_Tech"));
			//shadowMapEffects.add(EffectManager::Instance()->getEffectHandle("StdMesh_ZOnly_Tech"));
			//instanceEffects;//todo: create instance effects = EffectManager::Instance()->getEffectHandle("DetailedMesh_Shadowed_A_Glow_Tech");
		}
	}
	else
	{
		// Blend Shapes are used
		effects.add(EffectManager::Instance()->getEffectHandle("StdMeshBlendShapes_Shadowed_Tech"));
		shadowMapEffects.add(EffectManager::Instance()->getEffectHandle("StdMeshBlendShapes_ZOnly_Tech"));
		//instanceEffects;//todo: create instance effects = EffectManager::Instance()->getEffectHandle("DetailedMesh_Shadowed_A_Glow_Tech");
	}
}

// Builds a Mesh from the data in system memory
void Mesh::loadFromMeshCPU_needsRC(MeshCPU &mcpu, int &threadOwnershipMask)
{
	MaterialSetCPU *pmscpu = mcpu.m_hMaterialSetCPU.getObject<MaterialSetCPU>();
	PrimitiveTypes::UInt32 nMats = pmscpu->m_materials.m_size;

	// Draw controls -------------------------------------------------------
	m_bDrawControl = true;
	EPEVertexFormat format = updateGeoFromMeshCPU_needsRC(mcpu, threadOwnershipMask);
	/*
	// Index Buffer --------------------------------------------------------
	m_hIndexBufferGPU = VertexBufferGPUManager::Instance()->createIndexGPUBuffer(mcpu.m_hIndexBufferCPU, !mcpu.m_manualBufferManagement);
	*/
	// Material Set ------------------------------------------------------------
	m_hMaterialSetGPU  = VertexBufferGPUManager::Instance()->createMatSetGPUFromMatSetCPU(mcpu.m_hMaterialSetCPU);
	
	// Effect --------------------------------------------------------------
	
	m_effects.reset(nMats*2);
	m_zOnlyEffects.reset(nMats*2);
	m_instanceEffects.reset(nMats*2);
	bool hasBlendShapes = m_vertexBuffersGPUHs.m_size > 1;

	for (PrimitiveTypes::UInt32 im = 0; im < nMats; im++)
	{
		MaterialCPU &curMatCpu = pmscpu->m_materials[im];
		Handle hEffect, hShadowMapEffect, hInstanceEffect;
		
		m_effects.add(PEStaticVector<Handle, 4>());
		m_zOnlyEffects.add(PEStaticVector<Handle, 4>());
		m_instanceEffects.add(PEStaticVector<Handle, 4>());
		
		chooseEffects(curMatCpu, hasBlendShapes, format, m_effects[im], m_zOnlyEffects[im], m_instanceEffects[im]);
		if (m_effects[im].m_size)
			addComponent(m_effects[im][0]);
	}
}

EPEVertexFormat Mesh::updateGeoFromMeshCPU_needsRC(MeshCPU &mcpu, int &threadOwnershipMask)
{
	assert(threadOwnershipMask & Threading::RenderContext); // make sure thread currently has RenderContext

	assert(mcpu.m_hPositionBufferCPU.isValid());
	assert(mcpu.m_hIndexBufferCPU.isValid());

	EPEVertexFormat res = PEVertexFormat_Count;
	MaterialSetCPU &matSetCpu = *mcpu.m_hMaterialSetCPU.getObject<MaterialSetCPU>();
	MaterialSetCPU::MaterialSetStats &matSetStats = matSetCpu.m_stats;
	
	// Index Buffer --------------------------------------------------------
	if (m_hIndexBufferGPU.isValid())
	{
		IndexBufferGPU *pib = m_hIndexBufferGPU.getObject<IndexBufferGPU>();
		pib->~IndexBufferGPU(); // this will release gpu memory
		m_hIndexBufferGPU.release();
	}
	
	for (unsigned int i = 0; i < m_vertexBuffersGPUHs.m_size; i++)
	{
		Handle &hvb = m_vertexBuffersGPUHs[i];
		VertexBufferGPU *pvb = hvb.getObject<VertexBufferGPU>();
		pvb->~VertexBufferGPU(); // this will release gpu memory
		hvb.release();
	}

    PE::IRenderer::checkForErrors("");

	m_hIndexBufferGPU = VertexBufferGPUManager::Instance()->createIndexGPUBuffer(mcpu.m_hIndexBufferCPU, !mcpu.m_manualBufferManagement);

    PE::IRenderer::checkForErrors("");

	// TexCoord Buffer ---------------------------------------------------------
	m_hTexCoordBufferCPU = mcpu.m_hTexCoordBufferCPU;

	// Vertex Buffer -------------------------------------------------------
	Handle m_hVertexBufferGPU;
	m_hPositionBufferCPU = mcpu.m_hPositionBufferCPU;
	m_hNormalBufferCPU = mcpu.m_hNormalBufferCPU;
	m_hTangentBufferCPU = mcpu.m_hTangentBufferCPU;
	m_hSkinWeightsCPU = mcpu.m_hSkinWeightsCPU;

	// There are normals per vertex
	if (mcpu.m_hNormalBufferCPU.isValid())
	{
		if (mcpu.m_hTangentBufferCPU.isValid())
		{
			// there are tangents per vertex too
			// this fits DetailedMesh, but do we have textures for detailed mesh?
			if (matSetStats.hasAllNormalMaps)
			{
				// normal maps are present, can use all geo
				if (mcpu.m_hSkinWeightsCPU.isValid())
					res = PEVertexFormat_DetailedSkin;
				else
					res = PEVertexFormat_DetailedMesh;
			}
			else
			{
				PEWARN("Geometry for mesh has DetailedMesh data, but textures provided dont have normal map, so discarding tangent buffer data");

				if (mcpu.m_hSkinWeightsCPU.isValid())
					res = PEVertexFormat_StdSkin;
				else
					res = PEVertexFormat_StdMesh;
			}
	#if APIABSTRACTION_OGL && OGL_USE_VERTEX_BUFFER_ARRAYS
			m_hVertexBufferGPU.getObject<VertexBufferGPU>()->linkIndexBuffer(*m_hIndexBufferGPU.getObject<IndexBufferGPU>());
	#endif
		}
		else
		{
			// normals per vertex, but no tangents per vertex
			// this geo level is StdMesh
			// check material to make sure we are not passing in detailed textures
			if (matSetStats.setHasOneNormalMap)
			{
				PEWARN("Mesh has StdMesh geometry but material set has Normal map. The normal map will not be used");
				matSetCpu.removeMapsOfFamily(TextureFamily::NORMAL_MAP);
			}

			if (mcpu.m_hSkinWeightsCPU.isValid())
				res = PEVertexFormat_StdSkin;
			else
				res = PEVertexFormat_StdMesh;
		}
	}
	else
	{
		// no normals.
		// for now we only allow minimal colored meshes
		if (mcpu.m_hColorBufferCPU.isValid())
		{
			if (matSetStats.setHasOneNormalMap)
			{
				PEWARN("Mesh has MinimalColoredMesh geometry but material set has Normal map. The normal map will not be used");
				matSetCpu.removeMapsOfFamily(TextureFamily::NORMAL_MAP);
			}
			if (mcpu.m_hTangentBufferCPU.isValid())
				PEWARN("Geometry for mesh has tangent data, but no normal data, so discarding tangent buffer data");
			if (mcpu.m_hSkinWeightsCPU.isValid())
				PEWARN("Geometry for mesh has skin data, but no normal data so we are choosing minimal color buffer mesh, that doesnt have skinning, so discarding skinning buffer data");

			res = PEVertexFormat_ColoredMinimalMesh;
		}
		else
		{
			PEASSERT(0, "Geometry can't be created. Unsupported geometry format");
		}
	}
    
    PE::IRenderer::checkForErrors("");

	switch (res)
	{
	case PEVertexFormat_ColoredMinimalMesh:
		{
			m_hVertexBufferGPU = VertexBufferGPUManager::Instance()->createFromSource_ColoredMinimalMesh(
				mcpu.m_hPositionBufferCPU, 
				mcpu.m_hColorBufferCPU, 
				!mcpu.m_manualBufferManagement);
		}
		break;
	case PEVertexFormat_StdMesh:
		{
			m_hVertexBufferGPU = VertexBufferGPUManager::Instance()->createGPUBuffer(
				mcpu.m_hPositionBufferCPU, 
				mcpu.m_hTexCoordBufferCPU, 
				mcpu.m_hNormalBufferCPU,
				!mcpu.m_manualBufferManagement);
		}
		break;
	case PEVertexFormat_DetailedMesh:
		{
			m_hVertexBufferGPU = VertexBufferGPUManager::Instance()->createGPUBufferFromVBufTCBufNBufTBuf(
				mcpu.m_hPositionBufferCPU, 
				mcpu.m_hTexCoordBufferCPU, 
				mcpu.m_hNormalBufferCPU,
				mcpu.m_hTangentBufferCPU,
				!mcpu.m_manualBufferManagement);
		}
		break;
	case PEVertexFormat_StdSkin:
		{
			m_hVertexBufferGPU = VertexBufferGPUManager::Instance()->createGPUBufferFromVBufTCBufSWBufNBuf(
				m_hPositionBufferCPU, 
				m_hTexCoordBufferCPU,
				m_hSkinWeightsCPU,
				m_hNormalBufferCPU,
				!mcpu.m_manualBufferManagement
				);
		}
		break;
	case PEVertexFormat_DetailedSkin:
		{
			m_hVertexBufferGPU = VertexBufferGPUManager::Instance()->createGPUBufferFromVBufTCBufSWBufNBufTBuf(
				m_hPositionBufferCPU, 
				m_hTexCoordBufferCPU,
				m_hSkinWeightsCPU,
				m_hNormalBufferCPU,
				mcpu.m_hTangentBufferCPU,
				!mcpu.m_manualBufferManagement
				);
		}
		break;
	}
	

	// local cache
	
	m_vertexBuffersGPUHs.clear();
	m_vertexBuffersGPUHs.add(m_hVertexBufferGPU);

	bool hasBlendShapes = false;
	// add blend shapes if any
	for (PrimitiveTypes::UInt32 i = 0; i < mcpu.m_hAdditionalVertexBuffersCPU.m_size; i++)
	{
		Handle hAdditionalVertexBufferGPU = VertexBufferGPUManager::Instance()->createGPUBuffer(
			mcpu.m_hAdditionalVertexBuffersCPU[i], 
			mcpu.m_hAdditionalTexCoordBuffersCPU[i], 
			mcpu.m_hAdditionalNormalBuffersCPU[i],
			!mcpu.m_manualBufferManagement);

		hasBlendShapes = true;

		// local cache
		m_vertexBuffersGPUHs.add(hAdditionalVertexBufferGPU);
		hAdditionalVertexBufferGPU.getObject<VertexBufferGPU>()->m_weight = 0.0f; // set other vertex buffers to 0 weight
	}

	IRenderer::checkForErrors("update from geo\n");

	return res;
}

void Mesh::AddParticleEmitter() {
	//MaterialSetCPU* pmscpu = mcpu.m_hMaterialSetCPU.getObject<MaterialSetCPU>();
	//PrimitiveTypes::UInt32 nMats = pmscpu->m_materials.m_size;
    m_instanceEffects[0].add(EffectManager::Instance()->getEffectHandle("Particle_Pass0_CS_Tech"));
    m_instanceEffects[0].add(EffectManager::Instance()->getEffectHandle("Particle_CS_Tech"));
    m_instanceEffects[0].add(EffectManager::Instance()->getEffectHandle("DetailedMesh_Particle_Tech"));
    m_hIndexBufferGPU.m_dbgName = "ParticleIndexBufferGPU";
    m_isParticleEmitter = true;
}

PrimitiveTypes::Bool Mesh::hasPushedEffects() 
{
	IndexBufferGPU *pibGPU = m_hIndexBufferGPU.getObject<IndexBufferGPU>();
	PrimitiveTypes::UInt32 numTechniquesNeeded = pibGPU->m_indexRanges.m_size;
	
	return m_effects.m_size > numTechniquesNeeded;
}
	
void Mesh::popEffects()
{
	MeshHelpers::popEffects(this);
}


void Mesh::overrideEffects(Handle hNewEffect)
{
	MeshHelpers::setEffectOfTopEffectSecuence(this, hNewEffect);
}

void Mesh::overrideZOnlyEffects(Handle hNewEffect)
{
	MeshHelpers::setZOnlyEffectOfTopEffectSecuence(this, hNewEffect);
}

}; // namespace Components
}; // namespace PE
