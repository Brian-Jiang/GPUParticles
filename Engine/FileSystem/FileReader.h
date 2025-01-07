#ifndef __PYENGINE_2_0_FILE_READER_H__
#define __PYENGINE_2_0_FILE_READER_H__
// This class abstracts reading a file from HDD

// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

// Outer-Engine includes
#include <assert.h>
#if APIABSTRACTION_D3D9 | APIABSTRACTION_D3D11 | APIABSTRACTION_OGL
#include <iostream>
#include <fstream>
#endif

#define PE_USE_C_STYLE_FILE_READ 1

// Inter-Engine includes

#include "PrimeEngine/Utils/PEClassDecl.h"


// Sibling/Children includes

struct FileReader : public PE::PEAllocatableAndDefragmentable
{
	FileReader(const char *filename);

	~FileReader();

	void nextInt32(PrimitiveTypes::Int32 &dest);

	void nextFloat32(PrimitiveTypes::Float32 &dest);

	bool nextNonEmptyLine(char *dest, PrimitiveTypes::Int32 max);
	
	void readIntoBuffer(char *&pdata, PrimitiveTypes::UInt32 &size);

	// utils
	static unsigned int getFileLen(char *filename);
	static long LoadFile(char *filename, unsigned int bytes, unsigned char *buffer);

	#if PE_USE_C_STYLE_FILE_READ
		FILE *fr;
	#else
		std::ifstream m_file;
	#endif
};

#endif
