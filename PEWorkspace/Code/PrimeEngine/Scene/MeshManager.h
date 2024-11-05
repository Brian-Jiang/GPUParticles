#ifndef __PYENGINE_2_0_MESHMANAGER_H__
#define __PYENGINE_2_0_MESHMANAGER_H__

#define NOMINMAX
// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

// Outer-Engine includes
#include <assert.h>

// Inter-Engine includes
#include "PrimeEngine/MemoryManagement/Handle.h"
#include "PrimeEngine/PrimitiveTypes/PrimitiveTypes.h"
#include "../Events/Component.h"
#include "../Utils/Array/Array.h"
#include "../Geometry/MeshCPU/MeshCPU.h"
#include "../Math/Matrix4x4.h"

#include "PrimeEngine/APIAbstraction/GPUBuffers/VertexBufferGPU.h"
#include "PrimeEngine/APIAbstraction/GPUBuffers/IndexBufferGPU.h"

#include "PrimeEngine/APIAbstraction/Effect/Effect.h"

// Sibling/Children includes

namespace PE {
struct MaterialSetCPU;
namespace Components {

struct MeshManager : Component
{
	PE_DECLARE_CLASS(MeshManager);
	MeshManager(PE::GameContext &context, PE::MemoryArena arena, Handle hMyself);

	PE::Handle getAsset(const char *asset, const char *package, int &threadOwnershipMask);

	// for when asset is manually added from outside. it will get autogeenrated key
	void registerAsset(const Handle &h);

	PEMap<PE::Handle> m_assets;
};

}; // namespace Components
}; // namespace PE
#endif
