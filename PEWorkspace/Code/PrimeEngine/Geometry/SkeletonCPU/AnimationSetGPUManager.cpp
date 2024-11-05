
// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

#include "AnimationSetGPUManager.h"

#include "PrimeEngine/APIAbstraction/GPUBuffers/AnimSetBufferGPU.h"

namespace PE {
Handle AnimationSetGPUManager::s_myHandle;


Handle AnimationSetGPUManager::ReadAnimationSet(PE::GameContext &context, PE::MemoryArena arena, const char *filename, const char *package, SkeletonCPU &skel)
{
	Handle res = m_map.findHandle(filename);
	if (res.isValid())
	{
		// already have it
		return res;
	}

	// first read cpu version and then create gpu
	Handle hCpu("ANIMATION_SET_CPU", sizeof(AnimationSetCPU));
	AnimationSetCPU *pAnimSet = new(hCpu) AnimationSetCPU(context, arena);
	pAnimSet->ReadAnimationSet(filename, package, skel);

	res  = Handle("AnimSetBufferGPU", sizeof(AnimSetBufferGPU));
	AnimSetBufferGPU *pasgpu = new(res) AnimSetBufferGPU(context, arena);
	pasgpu->createGPUBufferFromAnimSet(*pAnimSet);
	pasgpu->m_hAnimationSetCPU = hCpu;
	m_map.add(filename, res);
	return res;
}


}; // namespace PE
