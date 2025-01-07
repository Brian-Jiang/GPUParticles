#define NOMINMAX

// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

// immediate include
#include "TexCoordBufferCPU.h"

// Outer-Engine includes

// Inter-Engine includes

#include "PrimeEngine/FileSystem/FileReader.h"
#include "PrimeEngine/Utils/StringOps.h"
#include "PrimeEngine/MainFunction/MainFunctionArgs.h"
// Sibling/Children includes


// Reads the specified buffer from file
void TexCoordBufferCPU::ReadTexCoordBuffer(const char *filename, const char *package)
{
    PEString::generatePathname(*m_pContext, filename, package, "TexCoordBuffers", PEString::s_buf, PEString::BUF_SIZE);
	
	// Path is now a full path to the file with the filename itself
	FileReader f(PEString::s_buf);

	char line[256];
	f.nextNonEmptyLine(line, 255);
	// TODO : make sure it is "TEXCOORD_BUFFER"
	PrimitiveTypes::Int32 n;
	f.nextInt32(n);
	m_values.reset(n * 2); // 2 Float32 per vertex

	// Read all values
	PrimitiveTypes::Float32 val;
	for (int i = 0; i < n * 2; i++)
	{
		f.nextFloat32(val);
		m_values.add(val);
	}
}

void TexCoordBufferCPU::createBillboardCPUBuffer()
{
	m_values.reset(2 * 4);
	m_values.add(1.0f); m_values.add(1.0f);
	m_values.add(1.0f); m_values.add(0.0f);
	m_values.add(0.0f); m_values.add(0.0f);
	m_values.add(0.0f); m_values.add(1.0f);
}

void TexCoordBufferCPU::createMockCPUBuffer(PrimitiveTypes::UInt32 nVertices)
{
	m_values.reset(2 * nVertices);
	
	for (PrimitiveTypes::UInt32 iv = 0; iv < nVertices; iv++)
	{
		m_values.add(0.0f);
		m_values.add(0.0f);
	}
	
}
