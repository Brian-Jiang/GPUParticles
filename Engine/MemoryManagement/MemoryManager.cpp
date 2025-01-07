
// Outer-Engine includes

// Inter-Engine includes
#include "PrimeEngine/Utils/StringOps.h"

// Sibling/Children includes
#include "MemoryManager.h"

#if PE_PLAT_IS_PSVITA
#endif

MemoryManager *MemoryManager::s_pInstance = 0;

void *MemoryManager::nextAlligned(void *ptr)
{
	void *allignedPtr = ptr;
	unsigned int modAliign = (uintptr_t)ptr % ALLIGNMENT;
	if (modAliign)
	{
		// ptr is not ALLIGNMENT alligned
		allignedPtr = (void *)((uintptr_t)allignedPtr + ALLIGNMENT - modAliign);
	}
	return allignedPtr;
}
void MemoryManager::Construct()
{
	unsigned int totalMemoryNeeded = 0;
	for (unsigned int i = 0; i < N_MEMORY_POOLS; i++)
	{
		// fill in every memory pool. memory pool has blocks of size g_memoryPools[i]
		unsigned int poolSize = MemoryPool::SpaceRequired(
			g_memoryPools[i][0], g_memoryPools[i][1]);

		totalMemoryNeeded += poolSize + ALLIGNMENT; // add allignment to make sure theres enough space for realligning a pointer after each pool
	}

	void *ptr = malloc(totalMemoryNeeded + ALLIGNMENT);

	PEINFO("Memory manager allocated memory %dB at 0x%p\n", totalMemoryNeeded + ALLIGNMENT, ptr);
	memset(ptr, 0, totalMemoryNeeded + ALLIGNMENT);
	MemoryManager::s_pInstance = new MemoryManager();
	void *allignedPtr = nextAlligned(ptr);
	
	for (unsigned int i = 0; i < N_MEMORY_POOLS; i++)
	{
		// fill in every memory pool. memory pool has blocks of size g_memoryPools[i]
		unsigned int poolSize = MemoryPool::SpaceRequired(
			g_memoryPools[i][0], g_memoryPools[i][1]);

		MemoryPool *pPool = MemoryPool::Construct(g_memoryPools[i][0], g_memoryPools[i][1], allignedPtr);
		
		// In the beginning, we are filling out each fourth memory pool
		// this is a support for the future, where multiple memory pools of the same size
		// will exist. at this point, we will use only one memory pool per a block size
		
		// m_memoryPools =
		// [
		// MemoryPool : with g_memoryPools[1] blocks of g_memoryPools[0],
		// null,
		// null,
		// null,
		// MemoryPool : with 1024 blocks of g_memoryPools[1],
		// ...
		// ]
		s_pInstance->m_memoryPools[i * 4] = pPool;

		allignedPtr = (void *)((uintptr_t)(allignedPtr) + poolSize);
		allignedPtr = nextAlligned(allignedPtr);
	}

	PEASSERT((char*)ptr + totalMemoryNeeded + ALLIGNMENT >= allignedPtr, "Error in alocating memory manager");
}

void MemoryManager::memoryReport(void *dest, unsigned int &size)
{
	char *start = (char *)(dest);
	char *cur = start;
	unsigned int curSize = 0;
	curSize = StringOps::writeToString("{'pools':[", cur, 256);

	for (unsigned int i = 0; i < N_MEMORY_POOLS; i++)
	{
		char buf[16];
		unsigned int memoryPoolIndex = i * 4;

		curSize += StringOps::writeToString("{'bs':", cur + curSize, 256);
		unsigned int blockSize = m_memoryPools[memoryPoolIndex]->getBlockSize();
		StringOps::intToStr(blockSize, buf, 16);
		curSize += StringOps::writeToString(buf, cur + curSize, 256);
		
		curSize += StringOps::writeToString(",'nb':", cur + curSize, 256);
		unsigned int totalNumBlocks = m_memoryPools[memoryPoolIndex]->getNumBlocks();
		StringOps::intToStr(totalNumBlocks, buf, 16);
		curSize += StringOps::writeToString(buf, cur + curSize, 256);
		
		curSize += StringOps::writeToString(",'nf':", cur + curSize, 256);
		unsigned int numFreeBlocks = m_memoryPools[memoryPoolIndex]->getNumFreeBlocks();
		StringOps::intToStr(numFreeBlocks, buf, 16);
		curSize += StringOps::writeToString(buf, cur + curSize, 256);
		
		curSize += StringOps::writeToString("},", cur + curSize, 256);
		
		
	}
	curSize += StringOps::writeToString("]}", cur + curSize, 256);
	size = curSize;
}

