#ifndef __PYENGINE_2_0_STRTOGPUHANDLEMAP_H__
#define __PYENGINE_2_0_STRTOGPUHANDLEMAP_H__

// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

// Outer-Engine includes
#include <assert.h>

// Inter-Engine includes
#include "PrimeEngine/MemoryManagement/Handle.h"
#include "PrimeEngine/PrimitiveTypes/PrimitiveTypes.h"
#include "PrimeEngine/Utils/StringOps.h"
#include "Array/Array.h"
// Sibling/Children includes

template <typename T>
struct StrTPair
{
	static const int StrSize = 256;
	char m_str[StrSize];
	T m_value;

	StrTPair(const char *pStr, T h)
		: m_value(h)
	{
		StringOps::writeToString(pStr, m_str, 256);
	}

	StrTPair(const StrTPair &cpy)
    :m_value(cpy.m_value)
	{
		StringOps::writeToString(cpy.m_str, m_str, 256);
	}

};

template <typename T>
struct PEMap : PE::PEAllocatable
{
	PEMap(PE::GameContext &context, PE::MemoryArena arena, PrimitiveTypes::UInt32 capacity) : m_pairs(context, arena, capacity)
	{
		m_arena = arena; m_pContext = &context;
	}

	bool add(const char *pKey, T v)
	{
		if (findIndex(pKey) != -1)
		{
			// add only if does not exist yet
			return false;
		}

		m_pairs.add(StrTPair<T>(pKey, v));
		return true;
	}

	// Searches for index of the key. Returns it if found, else returns -1
	PrimitiveTypes::Int32 findIndex(const char *pKey)
	{
		for (PrimitiveTypes::UInt32 i = 0; i < m_pairs.m_size; i++)
		{
			StrTPair<T> &curPair = m_pairs[i];
			if (StringOps::strcmp(pKey, curPair.m_str) == 0)
			{
				return i;
			}
		}
		return -1;
	}
	
	// Searches for handle paired with the key. Returns it if found, else returns handle with m_type = INVALID
	T &find(const char *pKey)
	{
		PrimitiveTypes::Int32 i = findIndex(pKey);
		if (i != -1)
		{
			return m_pairs[i].m_value;
		}
		assert(!"value not found");
		return *((T*)(0));
	}

	~PEMap()
	{
	}

	Array<StrTPair<T> > m_pairs;

	PE::MemoryArena m_arena; PE::GameContext *m_pContext;

};
#endif
