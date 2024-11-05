#ifndef __PYENGINE_2_0_SA_CopyInto_SysMem_FromInstancedObjectsAnimationCSReduceResult_H__
#define __PYENGINE_2_0_SA_CopyInto_SysMem_FromInstancedObjectsAnimationCSReduceResult_H__

// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

// Outer-Engine includes
#include <assert.h>

// Inter-Engine includes
#include "PrimeEngine/MemoryManagement/Handle.h"
#include "PrimeEngine/PrimitiveTypes/PrimitiveTypes.h"

#include "PrimeEngine/../../GlobalConfig/GlobalConfig.h"

// Sibling/Children includes
#include "ShaderAction.h"

namespace PE {

namespace Components{
struct Effect;
struct MeshInstance;
}
#if PE_API_IS_D3D11

struct SA_CopyInto_SysMem_FromInstancedObjectsAnimationCSReduceResult : ShaderAction
{
	SA_CopyInto_SysMem_FromInstancedObjectsAnimationCSReduceResult(
		ID3D11Buffer *pCSReduceComputeTargetBuffer, ID3D11Buffer *pCSReduceComputeTargetStagingBuffer,
		Components::MeshInstance *pSkinToAnalyzeFor, int instanceOffset, int numInstances,
		void *pAnalyzeToLocation, int analyzeToSize)
	 : m_pCSReduceComputeTargetBuffer(pCSReduceComputeTargetBuffer), m_pCSReduceComputeTargetStagingBuffer(pCSReduceComputeTargetStagingBuffer)
	 ,m_pSkinToAnalyzeFor(pSkinToAnalyzeFor), m_instanceOffset(instanceOffset), m_numInstances(numInstances)
	, m_pAnalyzeToLocation(pAnalyzeToLocation), m_analyzeToSize(analyzeToSize)
	{}
	virtual ~SA_CopyInto_SysMem_FromInstancedObjectsAnimationCSReduceResult(){}
    
	virtual void bindToPipeline(Components::Effect *pCurEffect = NULL) ;
	virtual void unbindFromPipeline(Components::Effect *pCurEffect = NULL) ;
	virtual void releaseData() ;

#	if APIABSTRACTION_D3D11
	ID3D11Buffer *m_pCSReduceComputeTargetBuffer;
	ID3D11Buffer *m_pCSReduceComputeTargetStagingBuffer;
#	endif
	Components::MeshInstance *m_pSkinToAnalyzeFor;
	int m_instanceOffset;
	int m_numInstances;
	
	void *m_pAnalyzeToLocation;
	int m_analyzeToSize;

};

#endif


}; // namespace PE
#endif
