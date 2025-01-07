#define NOMINMAX

// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

// Immediate include
#include "MaterialSetCPU.h"

// Outer-Engine includes

// Inter-Engine includes
#include "PrimeEngine/FileSystem/FileReader.h"
#include "PrimeEngine/Utils/StringOps.h"
#include "../../Utils/PEString.h"
#include "PrimeEngine/MainFunction/MainFunctionArgs.h"

namespace PE {

MaterialSetCPU::MaterialSetStats::MaterialSetStats()
{
	hasAllColorMaps = true;
	hasAllColorCubeMaps = true;
	hasAllNormalMaps = true;
	hasAllSpecularMaps = true;
	hasAllGlowMaps = true;
	
	setHasOneColorMap = false;
	setHasOneColorCubeMap = false;
	setHasOneNormalMap = false;
	setHasOneSpecularMap = false;
	setHasOneGlowMap = false;
}

// Reads the specified buffer from file
void MaterialSetCPU::ReadMaterialSet(const char *filename, const char *package)
{
    PEString::generatePathname(*m_pContext, filename, package, "MaterialSets", PEString::s_buf, PEString::BUF_SIZE);
	
	// Path is now a full path to the file with the filename itself
	FileReader f(PEString::s_buf);

	char line[256];
	f.nextNonEmptyLine(line, 255);
	// TODO : make sure it is "MATERIAL_SET"
	
	PrimitiveTypes::Int32 nMaterials;
	f.nextInt32(nMaterials);

	m_materials.reset(nMaterials);

	for (PrimitiveTypes::Int32 iMat = 0; iMat < nMaterials; iMat++)
	{
		char matFilename[256];
		f.nextNonEmptyLine(matFilename, 255);
		MaterialCPU matCPU(*m_pContext, m_arena);
		matCPU.ReadMaterial(matFilename, package);
		m_materials.add(matCPU);
	}
	
	prepareStats();
}

void MaterialSetCPU::createSetWithOneDefaultMaterial()
{
	m_materials.reset(1);
	MaterialCPU matCPU(*m_pContext, m_arena);
	matCPU.createDefaultMaterial();
	m_materials.add(matCPU);
	prepareStats();
}

void MaterialSetCPU::createSetWithOneTexturedMaterial(const char *textureFilename, const char *package, ESamplerState customSamplerState /* = SamplerState_Count*/)
{
	m_materials.reset(1);
	MaterialCPU matCPU(*m_pContext, m_arena);
	matCPU.createMaterialWithColorTexture(textureFilename, package, customSamplerState);
	m_materials.add(matCPU);
	prepareStats();
}

void MaterialSetCPU::createSetWithOneColorTextureArrayMaterial(const char * filenames[], PrimitiveTypes::UInt32 nFilenames, const char *package)
{
	m_materials.reset(1);
	MaterialCPU matCPU(*m_pContext, m_arena);
	matCPU.createMaterialWithOneColorTextureArrayMaterial(filenames, nFilenames, package);
	m_materials.add(matCPU);
	prepareStats();
}

void MaterialSetCPU::createSetWithOneTexturedMaterialWithGlow(const char *colorTextureFilename, const char *glowTextureFilename, const char *package)
{
	m_materials.reset(1);
	MaterialCPU matCPU(*m_pContext, m_arena);
	matCPU.createMaterialWithColorAndGlowTextures(colorTextureFilename, glowTextureFilename, package);
	m_materials.add(matCPU);
	prepareStats();
}

// analyze materials. based on what materials are present, vertex information might be discarded (will trigger warning)
// or maybe there is not enough geometry to use all material textures (will also trigger warning)
void MaterialSetCPU::prepareStats()
{
	PrimitiveTypes::UInt32 nMats = m_materials.m_size;

	for (PrimitiveTypes::UInt32 im = 0; im < nMats; im++)
	{
		MaterialCPU &curMatCpu = m_materials[im];
		
		PrimitiveTypes::Bool hasOneColorMap = false;
		PrimitiveTypes::Bool hasOneColorCubeMap = false;
		PrimitiveTypes::Bool hasOneNormalMap = false;
		PrimitiveTypes::Bool hasOneSpecularMap = false;
		PrimitiveTypes::Bool hasOneGlowMap = false;
	
		for (PrimitiveTypes::UInt32 itex = 0; itex < curMatCpu.m_textureFamilies.m_size; itex++)
		{
			if (curMatCpu.m_textureFamilies[itex] == TextureFamily::COLOR_MAP)
			{
				m_stats.setHasOneColorMap = hasOneColorMap = true;
			}
			else if (curMatCpu.m_textureFamilies[itex] == TextureFamily::COLOR_CUBE)
			{
				m_stats.setHasOneColorCubeMap = hasOneColorCubeMap = true;
			}
			else if (curMatCpu.m_textureFamilies[itex] == TextureFamily::NORMAL_MAP)
			{
				m_stats.setHasOneNormalMap = hasOneNormalMap = true;
			}
			else if (curMatCpu.m_textureFamilies[itex] == TextureFamily::SPECULAR_MAP)
			{
				m_stats.setHasOneSpecularMap = hasOneSpecularMap = true;
			}
			else if (curMatCpu.m_textureFamilies[itex] == TextureFamily::GLOW_MAP)
			{
				m_stats.setHasOneGlowMap = hasOneGlowMap = true;
			}
			else
			{
				PEASSERT(0, "Unknow texture type"); // error
			}
		}

		m_stats.hasAllColorMaps = m_stats.hasAllColorMaps && hasOneColorMap;
		m_stats.hasAllColorCubeMaps = m_stats.hasAllColorCubeMaps && hasOneColorCubeMap;
		m_stats.hasAllNormalMaps = m_stats.hasAllNormalMaps && hasOneNormalMap;
		m_stats.hasAllSpecularMaps = m_stats.hasAllSpecularMaps && hasOneSpecularMap;
		m_stats.hasAllGlowMaps = m_stats.hasAllGlowMaps && hasOneGlowMap;	
	}
}

void MaterialSetCPU::removeMapsOfFamily(TextureFamily::TextureFamily_ f)
{
	PrimitiveTypes::UInt32 nMats = m_materials.m_size;

	for (PrimitiveTypes::UInt32 im = 0; im < nMats; im++)
	{
		MaterialCPU &curMatCpu = m_materials[im];
		
		for (PrimitiveTypes::UInt32 itex = 0; itex < curMatCpu.m_textureFamilies.m_size; itex++)
		{
			if (curMatCpu.m_textureFamilies[itex] == f)
			{
				curMatCpu.m_textureFamilies.remove(itex);
				curMatCpu.m_textureFilenames.remove(itex);
				curMatCpu.m_texturePackages.remove(itex);
				curMatCpu.m_customSamplerStates.remove(itex);
				--itex;
			}
		}
	}
}


}; // namespace PE
