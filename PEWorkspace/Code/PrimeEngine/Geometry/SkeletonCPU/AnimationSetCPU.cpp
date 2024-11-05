#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

#include "AnimationSetCPU.h"

#include "PrimeEngine/FileSystem/FileReader.h"
#include "PrimeEngine/FileSystem/FileWriter.h"
#include "PrimeEngine/Utils/StringOps.h"

namespace PE {

void AnimSpeedSet::ReadAnimationSpeedSet(const char *filename, const char *package)
{
    PEString::generatePathname(*m_pContext, filename, package, "AnimationSpeedSets", PEString::s_buf, PEString::BUF_SIZE);

	// Path is now a full path to the file with the filename itself
	StringOps::writeToString(PEString::s_buf, m_filename, 1024);

	FileReader f(PEString::s_buf);

	char line[256];
	f.nextNonEmptyLine(line, 255);
	// TODO : make sure it is "DC_V64"

	PrimitiveTypes::Int32 numAnims;
	f.nextInt32(numAnims);
	assert(numAnims <= 64);

	m_size = numAnims;

	for (PrimitiveTypes::UInt32 iAnim = 0; iAnim < (PrimitiveTypes::UInt32)numAnims; iAnim++)
	{
		f.nextFloat32(m_v64[iAnim]);
	}
}

void AnimSpeedSet::saveBackToFile()
{
	FileWriter f(m_filename);
	f.writeString("DC_V64\n");

	f.writeInt32(m_size); f.writeEOL();


	for (PrimitiveTypes::UInt32 i = 0; i < m_size; i++)
	{
		f.writeFloat32(m_v64[i]); f.writeEOL();
	}
	
	// destructor will close the file
}

void AnimNameSet::ReadAnimationNameSet(const char *filename, const char *package)
{
    PEString::generatePathname(*m_pContext, filename, package, "AnimationNameSets", PEString::s_buf, PEString::BUF_SIZE);

    FileReader f(PEString::s_buf);

	char line[256];
	f.nextNonEmptyLine(line, 255);
	// TODO : make sure it is "DC_STRING64"

	PrimitiveTypes::Int32 numAnims;
	f.nextInt32(numAnims);
	assert(numAnims <= 64);

	m_size = numAnims;

	for (PrimitiveTypes::UInt32 iAnim = 0; iAnim < (PrimitiveTypes::UInt32)numAnims; iAnim++)
	{
		f.nextNonEmptyLine(line, 255);
		m_strings[iAnim].set(line);
	}
}

// Reads the specified buffer from file
void AnimationSetCPU::ReadAnimationSet(const char *filename, const char *package, SkeletonCPU &skel)
{
    PEString::generatePathname(*m_pContext, filename, package, "AnimationSets", PEString::s_buf, PEString::BUF_SIZE);
	
	StringOps::writeToString(filename, m_name, 256);

	// Path is now a full path to the file with the filename itself
	FileReader f(PEString::s_buf);

	char line[256];
	f.nextNonEmptyLine(line, 255);
	
	int version = 0;
	if (0 == StringOps::strcmp(line, "ANIMATION_SET_V1"))
	{
		version = 1;
	}
	else if (0 == StringOps::strcmp(line, "ANIMATION_SET_V2_TV3_RQUAT_SV3"))
	{
		version = 2;
	}
	else if (0 == StringOps::strcmp(line, "ANIMATION_SET_V3_TV3_RQUAT_SV3"))
	{
		version = 3;
	}

	float positionFactor = version == 0 ? (1.0f / 100.0f) : (1.0f);

	PrimitiveTypes::Int32 numAnims;
	f.nextInt32(numAnims);

	m_animations.reset(numAnims);

	for (PrimitiveTypes::UInt32 iAnim = 0; iAnim < (PrimitiveTypes::UInt32)numAnims; iAnim++)
	{
		m_animations.add(AnimationCPU(*m_pContext, m_arena));
		AnimationCPU &curAnim = m_animations[iAnim];
		curAnim.ReadAnimation(f, skel, positionFactor, version);
	}
}

}; // namespace PE
