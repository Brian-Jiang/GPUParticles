#ifndef __PYENGINE_2_0_MEMORYPOOL_H__
#define __PYENGINE_2_0_MEMORYPOOL_H__

#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

#include <assert.h>

#if !APIABSTRACTION_PS3 && !APIABSTRACTION_IOS && !PE_PLAT_IS_PSVITA && !PE_PLAT_IS_PS4
	#include <process.h>
	#include <stdlib.h>
#else
	#include <stdlib.h>
	#include <memory>
#endif
#include <string.h>

#define ALLIGNMENT 16

#define DEBUG_MEMORY_ALLOC_FREE_CONSISTENCY 1

namespace PE
{
enum MemoryArena
{
	MemoryArena_Client,
	MemoryArena_Server,
	MemoryArena_Count,
	MemoryArena_Invalid = MemoryArena_Count,
};

enum MemoryAllocationType
{
	MemoryAllocationType_Default,
	MemoryAllocationType_Handle,
	MemoryAllocationType_Count
};

void *pemalloc(MemoryArena arena, size_t size);
void pefree(MemoryArena arena, void *ptr);

void *pemallocAlligned(MemoryArena arena, size_t size, size_t allignment, int &offset);
void pefreeAlligned(MemoryArena arena, void *ptr, int offset);
};

// note: this class will be created on an allocated space
class MemoryPool
{
private:
	// union to make sure that first part of the struct takes up 16 bytes
	// so that memory is 16 byte aligned (given that object was created on aligned memory block)

#if !APIABSTRACTION_PS3 && !PE_PLAT_IS_PS4
	#pragma warning(disable : 4201)
#endif

	struct PoolHeader {
		void *m_blockStart;
		unsigned int 
			m_blockSize,
			m_nBlocks,
			m_nBlocksFree;
#if PE_64_BIT
		// pad to align to 16 B
		int m_pad[3]; // extra 12B
#endif
	};

#if !APIABSTRACTION_PS3 && !PE_PLAT_IS_PS4
	#pragma warning(default : 4201)
#endif

	PoolHeader m_header;


	// first part of this chunk willl be free block list
	// only first elements are really used as free block list
	unsigned int m_freeBlockList[1]; 
	
	// This object will be created by a factory.
	MemoryPool(unsigned int blockSize, unsigned int nBlocks)
	{
		m_header.m_blockSize = blockSize;
		m_header.m_nBlocks = nBlocks;
	}
	
public:
	static unsigned int SpaceRequired(unsigned int blockSize, unsigned int nBlocks)
	{
		return sizeof(PoolHeader) + nBlocks * sizeof(unsigned int) + blockSize * nBlocks;
	}
	// creates the memory pool on top of the preallocated memory
	static MemoryPool *Construct(unsigned int blockSize, unsigned int nBlocks, void *memStart);

	void *getBlockStart(unsigned int blockIndex)
	{
		// calculate offset and return start of the block
		return (void *)((uintptr_t)(m_header.m_blockStart) + (uintptr_t)(m_header.m_blockSize * blockIndex));
	}
	unsigned int getBlockSize(){ return m_header.m_blockSize;}
	unsigned int getNumFreeBlocks(){ return m_header.m_nBlocksFree;}
	unsigned int getNumBlocks(){ return m_header.m_nBlocks;}


	bool allocateBlock(unsigned int requiredSize, unsigned int &out_memoryBlockIndex)
	{
		assert(requiredSize);
		if (m_header.m_nBlocksFree > 0)
		{
			out_memoryBlockIndex = m_freeBlockList[--m_header.m_nBlocksFree];
			return true;
		}
		else
		{
			return false;
		}
	}

	void freeBlock(unsigned int memoryBlockIndex)
	{
		m_freeBlockList[m_header.m_nBlocksFree++] = memoryBlockIndex;
	}

	void clearBlock(unsigned int blockIndex)
	{
		memset(getBlockStart(blockIndex), 0, m_header.m_blockSize);
	}
};

#endif
