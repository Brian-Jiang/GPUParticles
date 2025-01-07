#define NOMINMAX
// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

// Outer-Engine includes

// Inter-Engine includes
#include "PrimeEngine/FileSystem/FileReader.h"
#include "PrimeEngine/APIAbstraction/GPUMaterial/GPUMaterialSet.h"
#include "PrimeEngine/PrimitiveTypes/PrimitiveTypes.h"
#include "PrimeEngine/APIAbstraction/Texture/Texture.h"
#include "PrimeEngine/APIAbstraction/Effect/EffectManager.h"
#include "PrimeEngine/APIAbstraction/GPUBuffers/VertexBufferGPUManager.h"
#include "Light.h"
#include "PrimeEngine/GameObjectModel/Camera.h"

// Sibling/Children includes
#include "LineMesh.h"
#include "SceneNode.h"
#include "CameraManager.h"
#include "../Lua/LuaEnvironment.h"
#include "PrimeEngine/Events/StandardEvents.h"
namespace PE {
namespace Components{

PE_IMPLEMENT_CLASS1(LineMesh, Mesh);

void LineMesh::addDefaultComponents()
{
	//add this handler before Mesh's handlers so we can intercept draw and modify transform
	PE_REGISTER_EVENT_HANDLER(Events::Event_GATHER_DRAWCALLS, LineMesh::do_GATHER_DRAWCALLS);
	Mesh::addDefaultComponents();
}

void LineMesh::loadFrom3DPoints_needsRC(float *vals, int numPoints, const char *techName, int &threadOwnershipMask)
{
	if (!m_meshCPU.isValid())
	{
		m_meshCPU = Handle("MeshCPU LineMesh", sizeof(MeshCPU));
		new (m_meshCPU) MeshCPU(*m_pContext, m_arena);
	}
	MeshCPU &mcpu = *m_meshCPU.getObject<MeshCPU>();

	if (!m_loaded)
		mcpu.createEmptyMesh();
	
	// this will cause not using the vertex buffer manager
	//so that engine always creates a new vertex buffer gpu and doesn't try to find and
	//existing one
	mcpu.m_manualBufferManagement = true;

	PositionBufferCPU *pVB = mcpu.m_hPositionBufferCPU.getObject<PositionBufferCPU>();
	ColorBufferCPU *pCB = mcpu.m_hColorBufferCPU.getObject<ColorBufferCPU>();
	IndexBufferCPU *pIB = mcpu.m_hIndexBufferCPU.getObject<IndexBufferCPU>();

	pIB->m_primitiveTopology = PEPrimitveTopology_LINES;
	pIB->m_verticesPerPolygon = 2;

	pVB->m_values.reset(numPoints * 3);
	pCB->m_values.reset(numPoints * 3);
	pIB->m_values.reset(numPoints);

	pIB->m_indexRanges[0].m_start = 0;
	pIB->m_indexRanges[0].m_end = numPoints - 1;
	pIB->m_indexRanges[0].m_minVertIndex = 0;
	pIB->m_indexRanges[0].m_maxVertIndex = numPoints - 1;

	pIB->m_minVertexIndex = pIB->m_indexRanges[0].m_minVertIndex;
	pIB->m_maxVertexIndex = pIB->m_indexRanges[0].m_maxVertIndex;

	for (int ip = 0; ip < numPoints; ip++)
	{
		float x = vals[ip*6+0];
		float y = vals[ip*6+1];
		float z = vals[ip*6+2];

		float r = vals[ip*6+3];
		float g = vals[ip*6+4];
		float b = vals[ip*6+5];


		pVB->m_values.add(x, y, z);
		
		pCB->m_values.add(r, g, b);
		pIB->m_values.add(ip);
	}

	if (!m_loaded)
	{
		// first time creating gpu mesh
		loadFromMeshCPU_needsRC(mcpu, threadOwnershipMask);
		
		Handle hEffect = EffectManager::Instance()->getEffectHandle("ColoredMinimalMesh_Line_A_0_Tech");

		for (int imat = 0; imat < m_effects.m_size; imat++)
		{
			if (m_effects[imat].m_size)
				m_effects[imat][0] = hEffect;
		}
		
		
		m_loaded = true;
	}
	else
	{
		//just need to update vertex buffers gpu
		updateGeoFromMeshCPU_needsRC(mcpu, threadOwnershipMask);
	}
}

void LineMesh::do_GATHER_DRAWCALLS(Events::Event *pEvt)
{

}


}; // namespace Components
}; // namespace PE
