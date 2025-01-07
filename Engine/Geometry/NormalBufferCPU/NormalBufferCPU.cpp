// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

#include "NormalBufferCPU.h"

NormalBufferCPU::NormalBufferCPU(PE::GameContext &context, PE::MemoryArena arena)
: m_values(context, arena, 0)
{
	m_pContext = &context;
}

// Reads the specified buffer from file
void NormalBufferCPU::ReadNormalBuffer(const char *filename, const char *package)
{
	PEString::generatePathname(*m_pContext, filename, package, "NormalBuffers", PEString::s_buf, PEString::BUF_SIZE);
	
	// Path is now a full path to the file with the filename itself
	FileReader f(PEString::s_buf);

	char line[256];
	f.nextNonEmptyLine(line, 255);
	// TODO : make sure it is "NORMAL_BUFFER"
	PrimitiveTypes::Int32 n;
	f.nextInt32(n);
	m_values.reset(n * 3); // 3 Float32 per normal

	// Read all values
	PrimitiveTypes::Float32 val;
	for (int i = 0; i < n * 3; i++)
	{
		f.nextFloat32(val);
		m_values.add(val);
	}

}

void NormalBufferCPU::createBillboardCPUBuffer()
{
	m_values.reset(3 * 4);
	m_values.add(0.0f); m_values.add(0.0f); m_values.add(1.0f);
	m_values.add(0.0f); m_values.add(0.0f); m_values.add(1.0f);
	m_values.add(0.0f); m_values.add(0.0f); m_values.add(1.0f);
	m_values.add(0.0f); m_values.add(0.0f); m_values.add(1.0f);
}
