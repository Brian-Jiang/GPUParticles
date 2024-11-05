#ifndef __PYENGINE_2_0_MESH_H__
#define __PYENGINE_2_0_MESH_H__

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

struct Mesh : public Component
{
	PE_DECLARE_CLASS(Mesh);

	// Constructor -------------------------------------------------------------
	Mesh(PE::GameContext &context, PE::MemoryArena arena, Handle hMyself);

	virtual ~Mesh(){}

	virtual void addDefaultComponents();

	// need this to maintain m_instances
	virtual void addComponent(Handle hComponent, int *pAllowedEvents = NULL);
	virtual void removeComponent(int index);


	// Methods -----------------------------------------------------------------

	// Builds a Mesh from the data in system memory
	void loadFromMeshCPU_needsRC(MeshCPU &mcpu, int &threadOwnershipMask);
	EPEVertexFormat updateGeoFromMeshCPU_needsRC(MeshCPU &mcpu, int &threadOwnershipMask);

    void AddParticleEmitter();

	// Component ------------------------------------------------------------

	// Individual events -------------------------------------------------------
	
	Handle &nextAdditionalShaderValue(int size)
	{
		m_additionalShaderValues.add(Handle("RAW_DATA", size));
		return m_additionalShaderValues[m_additionalShaderValues.m_size-1];
	}


	void overrideEffects(Handle newEffect);
	void overrideZOnlyEffects(Handle newEffect);
	

	void popEffects();

	PrimitiveTypes::Bool hasPushedEffects();
	// Member variables --------------------------------------------------------
	//Handle m_hVertexBufferGPU;
	Handle m_hTexCoordBufferCPU;
	
	Handle m_hIndexBufferGPU;
	
	Handle m_hMaterialSetGPU;

	PrimitiveTypes::Bool m_processShowEvt;

	Handle m_hPositionBufferCPU;
	Handle m_hNormalBufferCPU;
	Handle m_hTangentBufferCPU;

	Handle m_hSkinWeightsCPU;

	Array<Handle> m_additionalShaderValues;

	PEStaticVector<Handle, 4> m_vertexBuffersGPUHs;

	Array< PEStaticVector<Handle, 4> > m_effects;
	Array< PEStaticVector<Handle, 4> > m_zOnlyEffects;
	Array< PEStaticVector<Handle, 4> > m_instanceEffects;

	Array<Handle, 1> m_instances; // special cahce of instances
	Array<Handle> m_lods;
    int m_numVisibleInstances;
	
	Handle m_hAnimationSetGPU; // reference to animation stored in gpu buffer
	
	bool m_bDrawControl;
    
    bool m_performBoundingVolumeCulling;

	bool m_isParticleEmitter;
};

}; // namespace Components
}; // namespace PE
#endif
