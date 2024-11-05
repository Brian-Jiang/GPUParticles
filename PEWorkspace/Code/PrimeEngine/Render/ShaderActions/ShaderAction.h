#ifndef __PYENGINE_2_0_SHADER_ACTION_H__
#define __PYENGINE_2_0_SHADER_ACTION_H__

#define NOMINMAX
// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

// Outer-Engine includes
#include <assert.h>

// Inter-Engine includes
#include "PrimeEngine/Utils/PEClassDecl.h"
#include "PrimeEngine/MemoryManagement/Handle.h"

// Sibling/Children includes

namespace PE {

namespace Components{
struct Effect;
}

struct ShaderAction : PE::PEAllocatableAndDefragmentable
{
	virtual ~ShaderAction() {}

	virtual void bindToPipeline(Components::Effect *pCurEffect = NULL) = 0;
	virtual void releaseData() = 0;
	virtual void unbindFromPipeline(Components::Effect *pCurEffect = NULL) = 0;

	// used for OpenGL when instead of blitting whole memory, we blit one value at a time identfied by ascii name
	struct NameToData
	{
		const char *m_name;
		int m_dataOffset;
	};
};

}; // namespace PE
#endif
