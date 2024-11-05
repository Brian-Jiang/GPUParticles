
// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

// immediate include
#include "TangentBufferCPU.h"

// Outer-Engine includes

// Inter-Engine includes

#include "PrimeEngine/FileSystem/FileReader.h"
#include "PrimeEngine/Utils/StringOps.h"
#include "PrimeEngine/MainFunction/MainFunctionArgs.h"
// Sibling/Children includes

TangentBufferCPU::TangentBufferCPU(PE::GameContext &context, PE::MemoryArena arena) : m_values(context, arena, 0)
{
	m_pContext = &context;
}

// Reads the specified buffer from file
void TangentBufferCPU::ReadTangentBuffer(const char *filename, const char *package)
{
    PEString::generatePathname(*m_pContext, filename, package, "TangentBuffers", PEString::s_buf, PEString::BUF_SIZE);
	
	// Path is now a full path to the file with the filename itself
	FileReader f(PEString::s_buf);

	char line[256];
	f.nextNonEmptyLine(line, 255);
	// TODO : make sure it is "TANGENT_BUFFER"
	PrimitiveTypes::Int32 n;
	f.nextInt32(n);
	m_values.reset(n * 3); // 3 Float32 per a tangent

	// Read all values
	PrimitiveTypes::Float32 val;
	for (int i = 0; i < n * 3; i++)
	{
		f.nextFloat32(val);
		m_values.add(val);
	}
}

void TangentBufferCPU::createMockCPUBuffer(PrimitiveTypes::UInt32 nVertices)
{
	m_values.reset(3 * nVertices);
	
	for (PrimitiveTypes::UInt32 iv = 0; iv < nVertices; iv++)
	{
		m_values.add(0.0f);
		m_values.add(0.0f);
		m_values.add(0.0f);
	}
	
}

