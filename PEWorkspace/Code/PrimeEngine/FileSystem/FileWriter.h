#ifndef __PYENGINE_2_0_FILE_WRITER_H__
#define __PYENGINE_2_0_FILE_WRITER_H__
// This class abstracts reading a file from HDD

// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

// Outer-Engine includes
#include <assert.h>
#if APIABSTRACTION_D3D9 || APIABSTRACTION_D3D11 || APIABSTRACTION_OGL || PE_PLAT_IS_PSVITA
#include <iostream>
#include <fstream>
#endif

#include "PrimeEngine/Utils/PEClassDecl.h"

struct FileWriter : public PE::PEAllocatableAndDefragmentable
{
	FileWriter(const char *filename);

	~FileWriter();

	void writeInt32(PrimitiveTypes::Int32 val);

	void writeString(const char *s);

	void writeFloat32(PrimitiveTypes::Float32 val);

	void writeEOL();

#	if APIABSTRACTION_D3D9 || APIABSTRACTION_D3D11 || APIABSTRACTION_OGL || PE_PLAT_IS_PSVITA
		std::ofstream m_file;
#	endif

};

#endif
