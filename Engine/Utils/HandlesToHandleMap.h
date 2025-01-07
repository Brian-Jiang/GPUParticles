#ifndef __PYENGINE_2_0_HANDLESTOHANDLEMAP_H__
#define __PYENGINE_2_0_HANDLESTOHANDLEMAP_H__

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

template <int N>
struct HandlesHandlePair
{
    PE::Handle m_handles[N];
    PE::Handle m_handle;

	HandlesHandlePair(PE::Handle *pHandles, PE::Handle h)
	{
		for (PrimitiveTypes::UInt32 i = 0; i < N; i++)
		{
			m_handles[i] = pHandles[i];
		}
		m_handle = h;
	}

	HandlesHandlePair(const HandlesHandlePair &cpy)
	{
		for (PrimitiveTypes::UInt32 i = 0; i < N; i++)
		{
			m_handles[i] = cpy.m_handles[i];
		}
		m_handle = cpy.m_handle;
	}

	PrimitiveTypes::Bool hasKeys(PE::Handle * pHandles)
	{
		for (PrimitiveTypes::UInt32 i = 0; i < N; i++)
		{
			if (!(m_handles[i] == pHandles[i]))
				return false;
		}
		return true;
	}
};


template <int N>
struct HandlesToHandleMap : PE::PEAllocatableAndDefragmentable
{
	HandlesToHandleMap(PE::GameContext &context, PE::MemoryArena arena, PrimitiveTypes::UInt32 capacity) : m_pairs(context, arena, capacity)
	{
	}

	bool add(PE::Handle *pHandles, PE::Handle h)
	{
		if (findIndex(pHandles) != -1)
		{
			// add only if does not exist yet
			return false;
		}
		
		m_pairs.add(HandlesHandlePair<N>(pHandles, h));
		return true;
	}

	// Searches for index of the key. Returns it if found, else returns -1
	PrimitiveTypes::Int32 findIndex(PE::Handle *pHandles)
	{
		for (PrimitiveTypes::UInt32 i = 0; i < m_pairs.m_size; i++)
		{
			HandlesHandlePair<N> &curPair = m_pairs[i];
			if (curPair.hasKeys(pHandles))
			{
				return i;
			}
		}
		return -1;
	}
	
	// Searches for handle paired with the key. Returns it if found, else returns handle with m_type = INVALID
    PE::Handle findHandle(PE::Handle *pHandles)
	{
        PE::Handle res;
		PrimitiveTypes::Int32 i = findIndex(pHandles);
		if (i != -1)
		{
			res = m_pairs[i].m_handle;
		}
		return res;
	}

	~HandlesToHandleMap()
	{
	}

	Array<HandlesHandlePair<N> > m_pairs;
};
#endif
