#ifndef __PYENGINE_2_0_MEMORYMANAGER_H__
#define __PYENGINE_2_0_MEMORYMANAGER_H__
#define NOMINMAX
#define MAX_NUM_BLOCKS_PER_POOL 1000000
// Outer-Engine includes
#if PE_PLAT_IS_PS3 || PE_PLAT_IS_PSVITA
	#include <stdlib.h>
	#include <kernel.h>
#else
	#include <memory.h>
	//#include <malloc.h>
#endif

#ifdef _WIN32
#define _WINSOCKAPI_   /* Prevent inclusion of winsock.h in windows.h */
#include <windows.h>
#endif
// Inter-Engine includes

// Sibling/Children includes
#include "MemoryPool.h"

#include "PrimeEngine/APIAbstraction/Threading/Threading.h"

// this array holds block sizes that will be stored by memory pools
// 
#if PE_API_IS_D3D11
#define N_MEMORY_POOLS 25
#else
#define N_MEMORY_POOLS 24
#endif
const unsigned int g_memoryPools[][2] = {
	{16,       65536},  // we might increase 16 to 32 for 64 bit systems since Handle is 24B on 64 bit systems and we alin block sizes to 16
	{32,       4096},  //
	{48,       4096},  // need a lot of pieces of 40B to work with skin weights
	{64,       2048},  //
	{80,       2048},  //
	{96,       2048},  //
	{112,      2048},  //
	{128,      2048},  //
	{256,      2048},  //
	{512,      2048},  //
	{1024,     2048},  //
	{2048,     2048},  //
	{4096,     512},  //
	{8192,     512},  //
	{16384,    1024},  //
	{32768,    512},   //
	{65536,    256},   //
	{131072,   32},   //
	{262144,   16},    // 256KB ~ 256x256xRGBA              //256K * 
	{524288,   8},    // 512KB
	{1048576,  4},    // 1MB ~ 512x512xRGBA
	{2097152,  2},     // 2MB
	{4194304,  1},     // 4MB
	{4194304 * 2,  1},      // 8MB ~ 1024x1024xRGBA texture // 8MB * 2  =  16MB   SUM
	{4194304 * 4,  1},      // 16 MB                        // 16 * 1   =  16MB   SUM
};

class MemoryManager
{
private:
	static MemoryManager *s_pInstance;

	PE::Threading::Mutex m_allocMutex;
	
public:
	// indexes will be separated into groups holding same-sized block memory pools
	// i.e. [0-3] first size
	// [4-7]
	// [7-11]
	// [12-15]
	MemoryPool *m_memoryPools[1024]; // pointers to all memory pools

	static MemoryManager *instance() 
	{
		return MemoryManager::s_pInstance;
	}
	static void Construct();
	static void *nextAlligned(void *ptr);
	void *getBlockStart(unsigned int memoryPoolIndex, unsigned int blockIndex)
	{
		return m_memoryPools[memoryPoolIndex]->getBlockStart(blockIndex);
	}

	unsigned int getBlockSize(unsigned int memoryPoolIndex){ return m_memoryPools[memoryPoolIndex]->getBlockSize();}

	void allocateBlock(unsigned int requiredSize, unsigned int &out_memoryPoolIndex, unsigned int &out_memoryBlockIndex)
	{
		m_allocMutex.lock();
		
		bool allocated = false;
		for (unsigned int i = 0; i < N_MEMORY_POOLS; i++)
		{
			if (g_memoryPools[i][0] >= requiredSize)
			{
				// found memory pool index that stores this size
				out_memoryPoolIndex = i * 4;
				if (m_memoryPools[out_memoryPoolIndex]->allocateBlock(requiredSize, out_memoryBlockIndex))
				{
					allocated = true;
					break;
				}
				else
				{
					// TODO: print warning
					PEASSERT(false, "Ran out of this block size memory");
				}
			}
		}
		if (!allocated)
		{
			assert(!"Can't allocate memory pool. No more blocks left");
		}
		m_allocMutex.unlock();
	}

	void memoryReport(void *dest, unsigned int &size);

	void freeBlock(unsigned int memoryPoolIndex, unsigned int blockIndex)
	{
		m_allocMutex.lock();

		m_memoryPools[memoryPoolIndex]->freeBlock(blockIndex);

		m_allocMutex.unlock();
	}

	void clearBlock(unsigned int memoryPoolIndex, unsigned int blockIndex)
	{
		m_memoryPools[memoryPoolIndex]->clearBlock(blockIndex);
	}
	
};

#endif
