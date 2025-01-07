// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

// Outer-Engine includes

// Inter-Engine includes
#include "PrimeEngine/PrimitiveTypes/PrimitiveTypes.h"
#include "PrimeEngine/APIAbstraction/Effect/Effect.h"
#include "PrimeEngine/APIAbstraction/GPUBuffers/AnimSetBufferGPU.h"
#include "PrimeEngine/Scene/MeshInstance.h"
#include "PrimeEngine/Scene/Skeleton.h"
#include "PrimeEngine/Scene/DefaultAnimationSM.h"

#include "PrimeEngine/Scene/SkeletonInstance.h"


// Sibling/Children includes
#include "SA_CopyInto_SysMem_FromInstancedObjectsAnimationCSReduceResult.h"

namespace PE {
using namespace Components;
#if PE_API_IS_D3D11
void SA_CopyInto_SysMem_FromInstancedObjectsAnimationCSReduceResult::bindToPipeline(Effect *pCurEffect /* = NULL*/)
{
#	if APIABSTRACTION_D3D11
	
	/*typedef enum D3D11_MAP {
		D3D11_MAP_READ                 = 1,
		D3D11_MAP_WRITE                = 2,
		D3D11_MAP_READ_WRITE           = 3,
		D3D11_MAP_WRITE_DISCARD        = 4,
		D3D11_MAP_WRITE_NO_OVERWRITE   = 5 
	} D3D11_MAP;*/
	
	PEASSERT(m_pCSReduceComputeTargetBuffer != NULL, "This has to be set");
	{
		// we have m_pCSReduceComputeTargetBuffer which means we are using results of reduce
			
		D3D11Renderer *pd3d11Renderer = static_cast<D3D11Renderer *>(pCurEffect->m_pContext->getGPUScreen());

		int numInstances = m_numInstances;
		int numInstancesPerStep = PE_MAX_SKINED_INSTANCE_COUNT_IN_COMPUTE_CALL / 2 /*PE_MAX_NUM_OF_BUFFER_STEPS*/;

		PEASSERT(numInstances % numInstancesPerStep == 0, "Right now we only support a multiple of (instances per step). shouldnt be too hard to make general");
	
		int numSteps = numInstances / numInstancesPerStep;
	
		PEASSERT(numSteps == 1, "Debug assert for now");

		int numPalettesAtATime = numInstancesPerStep;

		int startMemOffset = m_instanceOffset * PE_MAX_BONE_COUNT_IN_DRAW_CALL * sizeof(Matrix4x4);

		int stepMemory = PE_MAX_BONE_COUNT_IN_DRAW_CALL * numPalettesAtATime * sizeof(Matrix4x4);

		PEASSERT(startMemOffset % stepMemory == 0, "Instance offset does not equal a multiple of steps");
	
		for (int iStep = 0; iStep < numSteps; ++iStep)
		{
			int startElement = startMemOffset + iStep *stepMemory;
			int endElement = startElement + stepMemory;
		
			D3D11_BOX box;
			box.back = 1; box.front = 0;
			box.top = 0; box.bottom = 1;
			box.left = startElement;
			box.right = endElement;
			pd3d11Renderer->m_pD3DContext->CopySubresourceRegion(
				m_pCSReduceComputeTargetStagingBuffer,
				0,
				0, // startElement,
				0,
				0,
				m_pCSReduceComputeTargetBuffer,
				0,
				&box);
		}

		D3D11_MAPPED_SUBRESOURCE mappedResource;
		HRESULT hr;
		while (true)
		{
			hr = pd3d11Renderer->m_pD3DContext->Map(
			m_pCSReduceComputeTargetStagingBuffer, // resource
			0, // subresource
			D3D11_MAP_READ, 
			0,//D3D11_MAP_FLAG_DO_NOT_WAIT, //0, 
			&mappedResource);

			if (SUCCEEDED(hr))
				break;

			if (hr == DXGI_ERROR_WAS_STILL_DRAWING)
				PEINFO("Waiting on Map()");
			else
				assert(SUCCEEDED(hr));
		}

		Matrix4x4 *pM = (Matrix4x4*)((int)(m_pAnalyzeToLocation));
		
		int numElements = PE_MAX_BONE_COUNT_IN_DRAW_CALL * numPalettesAtATime;
		PEASSERT(mappedResource.RowPitch == sizeof(Matrix4x4) * numElements, "size of mapped CS output is not matching");
		PEASSERT(m_analyzeToSize == sizeof(Matrix4x4) * numElements * numSteps, "analyze result must be converted to Matrix4x4");
	
		Matrix4x4 *pSrcMatrix = (Matrix4x4*)(mappedResource.pData);

		SkeletonInstance *pSkelInst = m_pSkinToAnalyzeFor->getFirstParentByTypePtr<SkeletonInstance>();
		Skeleton *pSkelObj = pSkelInst->getFirstParentByTypePtr<Skeleton>();

		SkeletonCPU *pSkel = pSkelObj->m_hSkeletonCPU.getObject<SkeletonCPU>();

		if (m_instanceOffset == 0)
		for (int iinst = 0; iinst < numPalettesAtATime; ++iinst) 
		{
			memcpy(pM, pSrcMatrix, sizeof(Matrix4x4) * PE_MAX_BONE_COUNT_IN_DRAW_CALL);
			pM += PE_MAX_BONE_COUNT_IN_DRAW_CALL;
			pSrcMatrix += PE_MAX_BONE_COUNT_IN_DRAW_CALL;
			//PEASSERT(pSkel->m_numJoints == PE_MAX_BONE_COUNT_IN_DRAW_CALL, "num joints doesnt match whats stored in buffer");
		}
		pd3d11Renderer->m_pD3DContext->Unmap(m_pCSReduceComputeTargetStagingBuffer, 0);
	}
#	endif
}

void SA_CopyInto_SysMem_FromInstancedObjectsAnimationCSReduceResult::unbindFromPipeline(Components::Effect *pCurEffect /* = NULL*/)
{}

void SA_CopyInto_SysMem_FromInstancedObjectsAnimationCSReduceResult::releaseData()
{
}

#endif

}; // namespace PE
