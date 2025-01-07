// Outer-Engine includes

#ifdef _WIN32
#define _WINSOCKAPI_   /* Prevent inclusion of winsock.h in windows.h */
#       include <windows.h>
#endif
#if PE_PLAT_IS_PSVITA
#include "kernel.h"
#endif
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"
#include "MemoryPool.h"
#include "PrimeEngine/Utils/ErrorHandling.h"
#include "PrimeEngine/../../GlobalConfig/GlobalConfig.h"

namespace PE 
{

void *pemalloc(MemoryArena arena, size_t size)
{
#if 0 && PE_PLAT_IS_PSVITA
#else
	assert(arena != MemoryArena_Invalid);
	return malloc(size);
#endif
	
}

void pefree(MemoryArena arena, void *ptr)
{
#if 0 && PE_PLAT_IS_PSVITA
#else
	free(ptr);
#endif
}

void *pemallocAlligned(MemoryArena arena, size_t size, size_t allignment, int &offset)
{
	void *ptr = pemalloc(arena, size + allignment - 1);
	offset = (allignment - (uintptr_t)(ptr) % allignment) % allignment;
	
	return (void *)((uintptr_t)(ptr) + offset);
}

void pefreeAlligned(MemoryArena arena, void *ptr, int offset)
{
	pefree(arena, (void *)((uintptr_t)(ptr) - offset));
}

};

// creates the memory pool on top of the preallocated memory
MemoryPool *MemoryPool::Construct(unsigned int blockSize, unsigned int nBlocks, void *memStart)
{
	#if	PE_PERFORM_REDUNDANCY_MEMORY_CHECKS
		PEASSERT(nBlocks % 4 == 0, "Num of block must be a multiple of 4!");
		PEASSERT(sizeof(void *) == 4, "Memory mamanger is written for 32 bit system, need to change logic below to allign properly");
	#endif
	MemoryPool *ptr = (MemoryPool *)memStart;
	ptr->m_header.m_blockSize = blockSize;
	ptr->m_header.m_nBlocks = nBlocks;
	ptr->m_header.m_nBlocksFree = nBlocks; // all blocks are free
	for (unsigned int i = 0; i < nBlocks; i++)
	{
		ptr->m_freeBlockList[i] = i;
	}

	//unsigned int ui = (unsigned int)(&(ptr->m_freeBlockList[0]));

	
	ptr->m_header.m_blockStart = (void *)((uintptr_t)(&(ptr->m_freeBlockList[0])) + (uintptr_t)(nBlocks * sizeof(unsigned int))); // skip occupancy table
	#if	PE_PERFORM_REDUNDANCY_MEMORY_CHECKS
		PEASSERT((int)(ptr->m_blockStart) % ALLIGNMENT == 0, "Address not alligned!");
	#endif
	return ptr;
}
