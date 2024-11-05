#ifndef __PYENGINE_2_0_HANDLE_H___
#define __PYENGINE_2_0_HANDLE_H___

// Outer-Engine includes

// Inter-Engine includes

// Sibling/Children includes
#include "MemoryManager.h"
#include "PrimeEngine/PrimitiveTypes/PrimitiveTypes.h"
#include "PrimeEngine/../../GlobalConfig/GlobalConfig.h"

//
#define PE_USE_CACHED_PTRS

#define INVALID_CHAR_PTR ((const char *)(-1))
#define INVALID_UINT 0xFFFFFFFF
namespace PE {

struct Handle
{
	void *m_cachedPtr;
	const char* m_dbgName;// -1 = invalid. 0 = no string stored; anything else points to a const string representing a debug name of the data stored handle

	union{
		unsigned int m_memoryPoolIndex;
		int m_memoryOffset;
	};
	unsigned int m_memoryBlockIndex;

	
	Handle() :
		m_cachedPtr(0),
		m_dbgName(INVALID_CHAR_PTR),
		m_memoryPoolIndex(INVALID_UINT),
		m_memoryBlockIndex(INVALID_UINT)
	{}
	

	Handle(const char *dbgName, unsigned int neededSize) :
		m_cachedPtr(0), m_dbgName(dbgName), 
			m_memoryPoolIndex(INVALID_UINT),
			m_memoryBlockIndex(INVALID_UINT)

	{
		if (neededSize > 0)
		{
			MemoryManager::instance()->allocateBlock(neededSize, m_memoryPoolIndex, m_memoryBlockIndex);
			cachePointer();
		}
	}
	void cachePointer() {
		m_cachedPtr = MemoryManager::instance()->getBlockStart(m_memoryPoolIndex, m_memoryBlockIndex);
		#if PE_PERFORM_REDUNDANCY_MEMORY_CHECKS
			PEASSERT((int)(m_cachedPtr) % 16 == 0, "Memory not alligned!");
		#endif
	}
	Handle(void *ptr)
	: m_memoryPoolIndex(INVALID_UINT)
	, m_memoryBlockIndex(INVALID_UINT)
	{
		m_cachedPtr = ptr;
		m_dbgName = 0;
	}
	// Copy constructor
	Handle(const Handle &other) :
		m_cachedPtr(other.m_cachedPtr),
		m_dbgName(other.m_dbgName),
		m_memoryPoolIndex(other.m_memoryPoolIndex),
		m_memoryBlockIndex(other.m_memoryBlockIndex)
		
	{}

	void recachePtr(){ m_cachedPtr = getObject();}

	template <typename T>
	T *getObject()
	{
		#ifdef PE_RUN_MEMORY_HANDLE_CHECKS
			// Check size
			if ((unsigned int)(getSize()) < sizeof(T))
			{
				assert(!"Retrieving object from handle with size bigger thatn stored by handle!");
			}
			if (!isValid())
			{
				assert(!"Retrieving object from invalid handle!");
			}
		#endif

		return (T *)(getObject());
	}

	void *getObject()
	{
		return m_cachedPtr;
		//return MemoryManager::instance()->getBlockStart(m_memoryPoolIndex, m_memoryBlockIndex);
	}
	const char* getDbgName(){return m_dbgName;}	

	PrimitiveTypes::Int32 getSize(){assert(m_memoryPoolIndex != INVALID_UINT); return MemoryManager::instance()->getBlockSize(m_memoryPoolIndex);}

	void release()
	{
		if (isValid())
		{
			MemoryManager::instance()->freeBlock(m_memoryPoolIndex, m_memoryBlockIndex);
			m_memoryPoolIndex = m_memoryBlockIndex = INVALID_UINT;
			m_dbgName = INVALID_CHAR_PTR;
		}
	}

	bool isValid() const {
		if (m_cachedPtr)
			return true;
		if (m_memoryPoolIndex != INVALID_UINT && m_memoryBlockIndex != INVALID_UINT)
			return true;
		return false;
	}

	bool operator==(const Handle &b)
	{
		return (m_cachedPtr == b.m_cachedPtr);
	}
};


}; // namespace PE
#endif
