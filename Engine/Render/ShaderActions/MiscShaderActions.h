#ifndef __PYENGINE_2_0_SHADER_VALUE_H__
#define __PYENGINE_2_0_SHADER_VALUE_H__

// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

// Outer-Engine includes
#include <assert.h>

// Inter-Engine includes
#include "PrimeEngine/MemoryManagement/Handle.h"
#include "PrimeEngine/PrimitiveTypes/PrimitiveTypes.h"

// Sibling/Children includes
#include "PrimeEngine/Render/ShaderActions/ShaderAction.h"

namespace PE {

namespace Components{
struct Effect;
}

// DX11-only shader actions

// stores a ptr to a vertex buffer and data that is copied into it
struct hlslDX11_SetVertexBufferValueFromHandleAction : ShaderAction
{
	virtual ~hlslDX11_SetVertexBufferValueFromHandleAction() {}

	virtual void bindToPipeline(Components::Effect *pCurEffect = NULL) ;
	virtual void unbindFromPipeline(Components::Effect *pCurEffect = NULL) ;
	virtual void releaseData() ;

#	if APIABSTRACTION_D3D11
		ID3D11Buffer *m_pBuffer; // the buffer that we will dump data into
		// data that we need to write into buffer. Note this data is not automatically released.
		// it is convenient for automatic data mirroring to GPU (this data could point to a PCU particle system or something like that)
		Handle m_data;
		int m_trueSize; // not all data in the data handle might be used. this size is the amount of data copied from handle
#	endif
};

// copies contents of resource buffer into a vertex buffer. Useful if results are generated in compute shaders and then need to be visualized
struct hlslDX11_SetVertexBufferValueFromResourceBufferAction : ShaderAction
{
	virtual ~hlslDX11_SetVertexBufferValueFromResourceBufferAction() {}

	virtual void bindToPipeline(Components::Effect *pCurEffect = NULL) ;
	virtual void unbindFromPipeline(Components::Effect *pCurEffect = NULL) ;
	virtual void releaseData() ;

#	if APIABSTRACTION_D3D11
	ResourceBufferGPU *pSrcResourceBufferGPU; // source data
	ID3D11Buffer *m_pBuffer; // the buffer that we will dump data into
#	endif
};


// copies contents of resource buffer into a resource buffer
// similar to previous class, but works with resource buffer as destination, not vertex buffer
struct hlslDX11_SetResourceBufferValueFromResourceBufferAction : ShaderAction
{
	virtual ~hlslDX11_SetResourceBufferValueFromResourceBufferAction() {}

	virtual void bindToPipeline(Components::Effect *pCurEffect = NULL) ;
	virtual void unbindFromPipeline(Components::Effect *pCurEffect = NULL) ;
	virtual void releaseData() ;

#	if APIABSTRACTION_D3D11
	ResourceBufferGPU *pSrcResourceBufferGPU; // source data
	ID3D11Buffer *m_pBuffer; // the buffer that we will dump data into
#	endif
};

}; // namespace PE
#endif
