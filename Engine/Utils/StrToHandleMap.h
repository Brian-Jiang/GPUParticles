#ifndef __PYENGINE_2_0_STRTOHANDLEMAP_H__
#define __PYENGINE_2_0_STRTOHANDLEMAP_H__

// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

// Outer-Engine includes
#include <assert.h>

// Inter-Engine includes
#include "PrimeEngine/Utils/PEClassDecl.h"
#include "PrimeEngine/MemoryManagement/Handle.h"
#include "PrimeEngine/PrimitiveTypes/PrimitiveTypes.h"
#include "PrimeEngine/Utils/StringOps.h"
#include "Array/Array.h"
// Sibling/Children includes

struct StrHandlePair
{
	char m_str[256];
    PE::Handle m_handle;

	StrHandlePair(const char *pStr, PE::Handle h)
	{
		StringOps::writeToString(pStr, m_str, 256);
		m_handle = h;
	}

	StrHandlePair(const StrHandlePair &cpy)
	{
		m_handle = cpy.m_handle;
		StringOps::writeToString(cpy.m_str, m_str, 256);
	}

};

struct StrToHandleMap : PE::PEAllocatable
{
	StrToHandleMap(PE::GameContext &context, PE::MemoryArena arena, PrimitiveTypes::UInt32 capacity) : m_pairs(context, arena, capacity)
	{
	}

	bool add(const char *pKey, PE::Handle h)
	{
		if (findIndex(pKey) != -1)
		{
			// add only if does not exist yet
			return false;
		}

		m_pairs.add(StrHandlePair(pKey, h));
		return true;
	}

	// Searches for index of the key. Returns it if found, else returns -1
	PrimitiveTypes::Int32 findIndex(const char *pKey)
	{
		for (PrimitiveTypes::UInt32 i = 0; i < m_pairs.m_size; i++)
		{
			StrHandlePair &curPair = m_pairs[i];
			if (StringOps::strcmp(pKey, curPair.m_str) == 0)
			{
				return i;
			}
		}
		return -1;
	}
	
	// Searches for handle paired with the key. Returns it if found, else returns handle with m_type = INVALID
    PE::Handle findHandle(const char *pKey)
	{
        PE::Handle res;
		PrimitiveTypes::Int32 i = findIndex(pKey);
		if (i != -1)
		{
			res = m_pairs[i].m_handle;
		}
		return res;
	}

	~StrToHandleMap()
	{
	}

	Array<StrHandlePair> m_pairs;
};
#endif
