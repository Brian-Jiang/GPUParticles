
// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

#include "FileWriter.h"

// Outer-Engine includes
#include <assert.h>

#include "PrimeEngine/MemoryManagement/Handle.h"
#include "PrimeEngine/PrimitiveTypes/PrimitiveTypes.h"
#include "PrimeEngine/Utils/StringOps.h"

FileWriter::FileWriter(const char *filename) : m_file(filename)
{
	assert(m_file.is_open());
}

FileWriter::~FileWriter()
{
	m_file.close();
}

void FileWriter::writeInt32(PrimitiveTypes::Int32 val)
{
	m_file << val;
}
void FileWriter::writeString(const char *s)
{
	m_file << s;
}

void FileWriter::writeFloat32(PrimitiveTypes::Float32 val)
{
	m_file << val;
}

void FileWriter::writeEOL()
{
	m_file << std::endl;
}

