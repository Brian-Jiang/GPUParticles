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
#include "TextMesh.h"
#include "SceneNode.h"
#include "CameraManager.h"
#include "../Lua/LuaEnvironment.h"
#include "PrimeEngine/Events/StandardEvents.h"
namespace PE {
namespace Components{

PE_IMPLEMENT_CLASS1(TextMesh, Mesh);

void TextMesh::addDefaultComponents()
{
	//add this handler before Mesh's handlers so we can intercept draw and modify transform
	PE_REGISTER_EVENT_HANDLER(Events::Event_GATHER_DRAWCALLS, TextMesh::do_GATHER_DRAWCALLS);
	Mesh::addDefaultComponents();
}

void TextMesh::loadFromString_needsRC(const char *str, const char *techName, int &threadOwnershipMask)
{
	int len = StringOps::length(str);

	if (!m_meshCPU.isValid())
	{
		m_meshCPU = Handle("MeshCPU TextMesh", sizeof(MeshCPU));
		new (m_meshCPU) MeshCPU(*m_pContext, m_arena);
	}
	MeshCPU &mcpu = *m_meshCPU.getObject<MeshCPU>();

	if (!m_loaded)
		mcpu.createBillboardMeshWithColorTexture("font512.dds", "Default", 32, 32, SamplerState_NoMips_NoMinTexelLerp_NoMagTexelLerp_Clamp);
	
	// this will cause not using the vertex buffer manager
	//so that engine always creates a new vertex buffer gpu and doesn't try to find and
	//existing one
	mcpu.m_manualBufferManagement = true;

	PositionBufferCPU *pVB = mcpu.m_hPositionBufferCPU.getObject<PositionBufferCPU>();
	TexCoordBufferCPU *pTCB = mcpu.m_hTexCoordBufferCPU.getObject<TexCoordBufferCPU>();
	NormalBufferCPU *pNB = mcpu.m_hNormalBufferCPU.getObject<NormalBufferCPU>();
	IndexBufferCPU *pIB = mcpu.m_hIndexBufferCPU.getObject<IndexBufferCPU>();
	pVB->m_values.reset(len * 4 * 3); // 4 verts * (x,y,z)
	pTCB->m_values.reset(len * 4 * 2);
	pNB->m_values.reset(len * 4 * 3);
	pIB->m_values.reset(len * 6); // 2 tris

	pIB->m_indexRanges[0].m_start = 0;
	pIB->m_indexRanges[0].m_end = len * 6 - 1;
	pIB->m_indexRanges[0].m_minVertIndex = 0;
	pIB->m_indexRanges[0].m_maxVertIndex = len * 4 - 1;

	pIB->m_minVertexIndex = pIB->m_indexRanges[0].m_minVertIndex;
	pIB->m_maxVertexIndex = pIB->m_indexRanges[0].m_maxVertIndex;

	float w = 32.f/2.0f;
	float h = 32.f;
	h = h / w;
	w = 1.0f;

	m_charW = w;
	m_charH = h;
	m_charWAbs = fabs(w);
	m_textLength = (float)(len);
	float curX = 0;
	float curY = 0;
	float pixSize2 = 1.0f / 512.0f / 2.0f;
	for (int ic = 0; ic < len; ic++)
	{
		char c = str[ic];
		int row = int(c) / 16;
		int column = int(c) % 16;
		float tcx = 1.0f / 16.0f * float(column);
		float tcy = 1.0f / 16.0f * float(row);
		pVB->m_values.add(curX, curY, 0); // top lef
		pVB->m_values.add(curX + w, curY, 0); // top right
		pVB->m_values.add(curX + w, curY-h, 0);
		pVB->m_values.add(curX, curY-h, 0);

		pIB->m_values.add(ic * 4 + 0, ic * 4 + 1, ic * 4 + 2);
		pIB->m_values.add(ic * 4 + 2, ic * 4 + 3, ic * 4 + 0);

		float dx = pixSize2;
		float dy = pixSize2;
		pTCB->m_values.add(tcx + dx , tcy + dy); // top left
		pTCB->m_values.add(tcx + 1.0f/16.0f - dx, tcy + dy); // top right
		pTCB->m_values.add(tcx + 1.0f/16.0f - dx, tcy + 1.0f/16.0f - dy);
		pTCB->m_values.add(tcx + dx, tcy + 1.0f/16.0f - dy);
		
		pNB->m_values.add(0, 0, 0);
		pNB->m_values.add(0, 0, 0);
		pNB->m_values.add(0, 0, 0);
		pNB->m_values.add(0, 0, 0);
		curX += w;
	}

	if (!m_loaded)
	{
		// first time creating gpu mesh
		loadFromMeshCPU_needsRC(mcpu, threadOwnershipMask);

		if (techName)
		{
			Handle hEffect = EffectManager::Instance()->getEffectHandle(techName);

			for (unsigned int imat = 0; imat < m_effects.m_size; imat++)
			{
				if (m_effects[imat].m_size)
					m_effects[imat][0] = hEffect;
			}
		}
		m_loaded = true;
	}
	else
	{
		//just need to update vertex buffers gpu
		updateGeoFromMeshCPU_needsRC(mcpu, threadOwnershipMask);
	}
}

void TextMesh::do_GATHER_DRAWCALLS(Events::Event *pEvt)
{

}


}; // namespace Components
}; // namespace PE
