#ifndef __PYENGINE_2_0_TEXCOORD_BUFFER_CPU_MANAGER__
#define __PYENGINE_2_0_TEXCOORD_BUFFER_CPU_MANAGER__

#define NOMINMAX

// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

// Outer-Engine includes

// Inter-Engine includes
#include "PrimeEngine/MemoryManagement/Handle.h"
#include "PrimeEngine/PrimitiveTypes/PrimitiveTypes.h"
#include "PrimeEngine/Math/Matrix4x4.h"
#include "PrimeEngine/FileSystem/FileReader.h"
#include "PrimeEngine/Utils/StringOps.h"
#include "../../Utils/Array/Array.h"
#include "../../Utils/StrToHandleMap.h"
#include "PrimeEngine/MainFunction/MainFunctionArgs.h"

// Sibling/Children includes
#include "TexCoordBufferCPU.h"

struct TexCoordBufferCPUManager : PE::PEAllocatableAndDefragmentable
{
	TexCoordBufferCPUManager(PE::GameContext &context, PE::MemoryArena arena) : m_map(context, arena, 1024)
	{
		m_arena = arena; m_pContext = &context;
	}

	static void Construct(PE::GameContext &context, PE::MemoryArena arena)
	{
		s_myHandle = PE::Handle("TEXCOORD_BUFFER_CPU_MANAGER", sizeof(TexCoordBufferCPUManager));
		/* TexCoordBufferCPUManager *pMan = */ new(s_myHandle) TexCoordBufferCPUManager(context, arena);
	}

	static TexCoordBufferCPUManager* Instance()
	{
		return s_myHandle.getObject<TexCoordBufferCPUManager>();
	}

	// Reads the specified buffer from file
    PE::Handle ReadTexCoordBuffer(const char *filename, const char *package, const char *tag)
	{
		char key[256];
		StringOps::concat(filename, tag, &key[0], 256);

        PE::Handle res = m_map.findHandle(key);
		if (res.isValid())
		{
			// already have it
			return res;
		}

		res = PE::Handle("TEXCOORD_BUFFER_CPU", sizeof(TexCoordBufferCPU));
		TexCoordBufferCPU *pvbcpu = new(res) TexCoordBufferCPU(*m_pContext, m_arena);

		pvbcpu->ReadTexCoordBuffer(filename);

		m_map.add(key, res);
		return res;
	}

	static PE::Handle s_myHandle;

	StrToHandleMap m_map;
	PE::MemoryArena m_arena; PE::GameContext *m_pContext;
};

#endif
