#ifndef __PYENGINE_2_0_DRAW_LIST_H__
#define __PYENGINE_2_0_DRAW_LIST_H__

// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

// Outer-Engine includes
#include <assert.h>

// Inter-Engine includes
#include "PrimeEngine/MemoryManagement/Handle.h"
#include "PrimeEngine/PrimitiveTypes/PrimitiveTypes.h"
#include "../Events/Component.h"
#include "../Utils/Array/Array.h"
#include "Mesh.h"
#include "Light.h"

#include "PrimeEngine/APIAbstraction/GPUBuffers/VertexBufferGPU.h"
#include "PrimeEngine/APIAbstraction/GPUBuffers/IndexBufferGPU.h"
//#include "PrimeEngine/APIAbstraction/Effect/Lighting.h"
#include "PrimeEngine/Events/StandardEvents.h"

// Sibling/Children includes

namespace PE {

namespace Components {

namespace DrawCallType
{
	enum DrawCallType_
	{
		VBUF_INDBUF,
		VBUF,
		COMPUTE,
        VBUF_INDBUF_INDIRECT,
        COMPUTE_INDIRECT,
	};
};

struct HandlePair
{
	Handle m_a;
	Handle m_b;
};

struct DrawList : public Component
{
	PE_DECLARE_CLASS(DrawList);

	static const PrimitiveTypes::UInt32 c_maxNumDrawCalls = 512;
	static const PrimitiveTypes::UInt32 c_maxNumVBuffers = 8;

	// Constructor -------------------------------------------------------------
	DrawList(PE::GameContext &context, PE::MemoryArena arena, Handle hMyself) 
	: Component(context, arena, hMyself)
	, m_dbgNames(context, arena)
	, m_effects(context, arena)
	, m_vertexBuffers(context, arena)
	, m_instanceVertexBuffers(context, arena)
	, m_indexBuffers(context, arena)
	, m_dispatchParams(context, arena)
	, m_indexRanges(context, arena)
	, m_boneSegmentIds(context, arena)
	, m_outputs(context, arena)
	, m_drawCallTypes(context, arena)
	, m_optimizedIndices(context, arena)
	, m_objects(context, arena, c_maxNumDrawCalls)
	, m_shaderValues(context, arena)
	, m_globalShaderValues(context, arena)
	, m_instanceCounts(context, arena)
	, m_instanceOffsets(context, arena)
	{
		m_hMyself = hMyself;
		m_numDrawCalls = 0;


	}
	virtual ~DrawList(){}

	// Singleton ---------------------------------------------------------------
	static void Construct(PE::GameContext &context, PE::MemoryArena arena)
	{
		for (PrimitiveTypes::UInt32 ibuf = 0; ibuf < 2; ibuf++)
		{
			s_normalLists[ibuf] = Handle("DRAW_LIST", sizeof(DrawList));
			DrawList *pDrawList = new(s_normalLists[ibuf]) DrawList(context, arena, s_normalLists[ibuf]);
			pDrawList->addDefaultComponents();

			s_zOnlyLists[ibuf] = Handle("DRAW_LIST", sizeof(DrawList));
			DrawList *pZOnlyDrawList = new(s_zOnlyLists[ibuf]) DrawList(context, arena, s_zOnlyLists[ibuf]);
			pZOnlyDrawList->addDefaultComponents();
		}
		m_curBuffer = 0;
	}

	static DrawList *Instance()
	{
		return s_normalLists[m_curBuffer].getObject<DrawList>();
	}

	static DrawList *ZOnlyInstance()
	{
		return s_zOnlyLists[m_curBuffer].getObject<DrawList>();
	}

	static DrawList *InstanceReadOnly()
	{
		return s_normalLists[(m_curBuffer + 1) % 2].getObject<DrawList>();
	}

	static DrawList *ZOnlyInstanceReadOnly()
	{
		return s_zOnlyLists[(m_curBuffer+1) % 2].getObject<DrawList>();
	}

	// Methods
	void reset();
	void beginDrawCallRecord(const char *dbgName);
	void setEffect(Handle hEffect);
	void setIndexBuffer(Handle hIndBuf, PrimitiveTypes::Int32 indBufRange = -1, PrimitiveTypes::Int32 boneSegmentRangIde = -1);
	void setInstanceCount(PrimitiveTypes::UInt32 count, PrimitiveTypes::UInt32 instanceOffset);

	void setDrawIndirect();
	void setDispatchIndirect();
	void setVertexBuffer(Handle hVertBuf);
	void setDispatchParams(Vector3 dispatch);
	void setInstanceVertexBuffer(Handle hInstVertBuf);
	void setStreamOutputVertexBuffer(Handle hVertBuf);
	
	Handle &nextShaderValue();
	Handle &nextShaderValue(int size); // allocation size for the value handle
	Handle &nextGlobalShaderValue();

	void optimize();
	void optimizeByVertexBuffer(Array<PrimitiveTypes::UInt32> &callIndices);


	void importMesh(Handle hMesh);
	void importSkin(Handle hSkin);
	// Component ------------------------------------------------------------

	virtual void addDefaultComponents()
	{
		Component::addDefaultComponents();

	}
	
	// Individual events -------------------------------------------------------
	virtual void do_RENDER(Events::Event *pEvt, int &threadOwnershipMask);

	virtual void do_RENDER_Z_ONLY(Events::Event *pEvt, int &threadOwnershipMask);


	void distributeEvtToAllObjects(Events::Event *pEvt)
	{
		for (PrimitiveTypes::UInt32 i = 0; i < m_objects.m_size; i++)
		{
			Component *pObj = m_objects[i].getObject<Component>();
			pObj->handleEvent(pEvt);
		}
	}

	static void swap() {m_curBuffer = (m_curBuffer + 1) % 2;}

	static void creteCustomZOnlyDrawList(PE::GameContext &context, PE::MemoryArena arena)
	{

		Handle ha("DRAW_LIST", sizeof(DrawList));
		DrawList *pZOnlyDrawListA = new(ha) DrawList(context, arena, ha);
		pZOnlyDrawListA->addDefaultComponents();

		Handle hb("DRAW_LIST", sizeof(DrawList));
		DrawList *pZOnlyDrawListB = new(hb) DrawList(context, arena, hb);
		pZOnlyDrawListB->addDefaultComponents();
	}

	static Handle s_normalLists[2]; // handle to itself
	static Handle s_zOnlyLists[2];

	static PrimitiveTypes::UInt32 m_curBuffer;
	Handle m_hMyself; // handle to itself
	Handle m_hParent;
	Handle m_hComponentParent;
	
	
	// Per Draw Call components
	PrimitiveTypes::UInt32 m_numDrawCalls;

	Array< const char *> m_dbgNames;
	Array<Handle> m_effects;
	Effect *m_pCurEffect;
	Array<Array<Handle> > m_vertexBuffers;
	VertexBufferGPU *m_pCurVertBuf;
	Array<Handle> m_instanceVertexBuffers;
	VertexBufferGPU *m_pCurInstanceVertBuf;
	Array<Handle> m_indexBuffers;
	Array<Vector3> m_dispatchParams;
	
	Array<PrimitiveTypes::Int32> m_indexRanges;
	Array<PrimitiveTypes::Int32> m_boneSegmentIds;
	Array<PrimitiveTypes::UInt32> m_instanceCounts;
	Array<PrimitiveTypes::UInt32> m_instanceOffsets;

	IndexBufferGPU *m_pCurIndBuf;
	Array<Handle> m_outputs; // for stream output
	VertexBufferGPU *m_pCurOutput;
	Array<DrawCallType::DrawCallType_> m_drawCallTypes;
	
	Array<PrimitiveTypes::UInt32> m_optimizedIndices;

	Array<Handle> m_objects;

	Array<Array<Handle> > m_shaderValues;

	Array<Handle> m_globalShaderValues;
};
}; // namespace Components
}; // namespace PE
#endif
