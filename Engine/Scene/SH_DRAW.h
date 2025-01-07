#ifndef __PYENGINE_2_0_SH_DRAW_H__
#define __PYENGINE_2_0_SH_DRAW_H__

// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

// Outer-Engine includes
#include <assert.h>

// Inter-Engine includes
#include "PrimeEngine/MemoryManagement/Handle.h"
#include "PrimeEngine/PrimitiveTypes/PrimitiveTypes.h"
#include "../Events/Component.h"
#include "../Events/StandardEvents.h"
#include "../Utils/Array/Array.h"
#include "PrimeEngine/Math/Frustrum.h"
#include "PrimeEngine/Math/Box.h"

struct IndexRange;

namespace PE {
struct IndexBufferGPU;

namespace Components {
struct Mesh;
struct DrawList;
struct Effect;
struct MeshInstance;

struct SingleHandler_DRAW : public Component
{
	PE_DECLARE_SINGLETON_CLASS(SingleHandler_DRAW);

	SingleHandler_DRAW(PE::GameContext &context, PE::MemoryArena arena, Handle hMyself) : Component(context, arena, hMyself)
	{}

	virtual ~SingleHandler_DRAW(){}

	virtual void addDefaultComponents()
	{
		Component::addDefaultComponents();
		PE_REGISTER_EVENT_HANDLER(Events::Event_GATHER_DRAWCALLS, SingleHandler_DRAW::do_GATHER_DRAWCALLS);
		PE_REGISTER_EVENT_HANDLER(Events::Event_GATHER_DRAWCALLS_Z_ONLY, SingleHandler_DRAW::do_GATHER_DRAWCALLS);
	}

	PE_DECLARE_IMPLEMENT_EVENT_HANDLER_WRAPPER(do_GATHER_DRAWCALLS);
	virtual void do_GATHER_DRAWCALLS(Events::Event *pEvt);

	static void Construct(PE::GameContext &context, PE::MemoryArena arena)
	{
		s_hMyself = Handle("RAW_DATA", sizeof(SingleHandler_DRAW));
		SingleHandler_DRAW *p = new(s_hMyself) SingleHandler_DRAW(context, arena, s_hMyself);
		p->addDefaultComponents();
	}

private:
	void gatherDrawCallsForRange(Mesh *pMeshCaller, DrawList *pDrawList, PE::Handle *pHVBs, int vbCount, Vector4 &vbWeights, int iRange,
		Events::Event_GATHER_DRAWCALLS *pDrawEvent, Events::Event_GATHER_DRAWCALLS_Z_ONLY *pZOnlyDrawEvent
	);

	void addNonInstancedTechShaderActions(Mesh *pMeshCaller, IndexRange &ir, int iBoneSegment, int iRenderGroup, int srcInstanceId, bool hasBoneSegments,
		DrawList *pDrawList, Effect *pEffect, const Matrix4x4 &evtProjectionViewWorldMatrix,
		int vbCount, Vector4 vbWeights);

	void addSAs_InstancedAnimationCSMap(PE::Components::DrawList *pDrawList, PE::Components::MeshInstance *pMeshInstance, PE::Components::Mesh *pMeshCaller, int numInstancesInGroup, int indexInInstanceList);

	void addSAs_InstancedAnimationCSReduce(PE::Components::DrawList *pDrawList, PE::Components::MeshInstance *pSkinCaller);

	void addSAa_InstancedAnimation_CSOnly_Pass2_and_CSCPU_Pass1_and_NoCS_Pass0(PE::Components::DrawList *pDrawList, PE::Components::Mesh *pMeshCaller,
		Matrix4x4 &evtProjectionViewWorldMatrix, int numInstancesInGroup,  int indexInInstanceList);

	void addSAa_InstancedAnimation_CSOnly_Pass2(PE::Components::DrawList *pDrawList);

	void addSAa_InstancedAnimation_NoCS_Pass0(PE::Components::DrawList *pDrawList, PE::Components::Mesh *pMeshCaller,
		Matrix4x4 &evtProjectionViewWorldMatrix, int numInstancesInGroup,  int indexInInstanceList);

	void addSAs_ParticleEmitter(PE::Components::DrawList* pDrawList);
	void addSAs_ParticleEmitter_Pass2(PE::Components::DrawList* pDrawList);

	bool checkMeshOcclusion(const Frustrum& frustrum, MeshInstance* meshInstance);
	bool checkBoxPlaneOnSameSide(const Box& box, const Matrix4x4& parent, const Vector3& point, const Vector3& normal);
};



struct PESSEH_CHANGE_TO_DEBUG_SHADER : public Component
{
	PE_DECLARE_SINGLETON_CLASS(PESSEH_CHANGE_TO_DEBUG_SHADER);

	PESSEH_CHANGE_TO_DEBUG_SHADER(PE::GameContext &context, PE::MemoryArena arena, Handle hMyself) : Component(context, arena, hMyself) {}
	virtual ~PESSEH_CHANGE_TO_DEBUG_SHADER(){}

	virtual void addDefaultComponents()
	{
		Component::addDefaultComponents();
		PE_REGISTER_EVENT_HANDLER(Events::Event_CHANGE_TO_DEBUG_SHADER, PESSEH_CHANGE_TO_DEBUG_SHADER::do_CHANGE_TO_DEBUG_SHADER);
	}

	PE_DECLARE_IMPLEMENT_EVENT_HANDLER_WRAPPER(do_CHANGE_TO_DEBUG_SHADER);
	virtual void do_CHANGE_TO_DEBUG_SHADER(Events::Event *pEvt);
	
	static void Construct(PE::GameContext &context, PE::MemoryArena arena)
	{
		s_hMyself = Handle("RAW_DATA", sizeof(PESSEH_CHANGE_TO_DEBUG_SHADER));
		PESSEH_CHANGE_TO_DEBUG_SHADER *p = new(s_hMyself) PESSEH_CHANGE_TO_DEBUG_SHADER(context, arena, s_hMyself);
		p->addDefaultComponents();
	}
};

struct PESSEH_POP_SHADERS : public Component
{
	PE_DECLARE_SINGLETON_CLASS(PESSEH_POP_SHADERS);

	PESSEH_POP_SHADERS(PE::GameContext &context, PE::MemoryArena arena, Handle hMyself) : Component(context, arena, hMyself) {}

	virtual ~PESSEH_POP_SHADERS(){}

	virtual void addDefaultComponents()
	{
		Component::addDefaultComponents();
		PE_REGISTER_EVENT_HANDLER(Events::Event_POP_SHADERS, PESSEH_POP_SHADERS::do_POP_SHADERS);
	}

	PE_DECLARE_IMPLEMENT_EVENT_HANDLER_WRAPPER(do_POP_SHADERS);
	virtual void do_POP_SHADERS(Events::Event *pEvt);

	static void Construct(PE::GameContext &context, PE::MemoryArena arena)
	{
		s_hMyself = Handle("RAW_DATA", sizeof(PESSEH_POP_SHADERS));
		PESSEH_POP_SHADERS *p = new(s_hMyself) PESSEH_POP_SHADERS(context, arena, s_hMyself);
		p->addDefaultComponents();
	}
};


struct PESSEH_DRAW_Z_ONLY : public Component
{
	PE_DECLARE_SINGLETON_CLASS(PESSEH_DRAW_Z_ONLY);

	PESSEH_DRAW_Z_ONLY(PE::GameContext &context, PE::MemoryArena arena, Handle hMyself) : Component(context, arena, hMyself)
	{}

	virtual ~PESSEH_DRAW_Z_ONLY(){}

	virtual void addDefaultComponents()
	{
		Component::addDefaultComponents();
		PE_REGISTER_EVENT_HANDLER(Events::Event_GATHER_DRAWCALLS_Z_ONLY, PESSEH_DRAW_Z_ONLY::do_GATHER_DRAWCALLS_Z_ONLY);
	}

	PE_DECLARE_IMPLEMENT_EVENT_HANDLER_WRAPPER(do_GATHER_DRAWCALLS_Z_ONLY);
	virtual void do_GATHER_DRAWCALLS_Z_ONLY(Events::Event *pEvt);

	static void Construct(PE::GameContext &context, PE::MemoryArena arena)
	{
		s_hMyself = Handle("RAW_DATA", sizeof(PESSEH_DRAW_Z_ONLY));
		PESSEH_DRAW_Z_ONLY *p = new(s_hMyself) PESSEH_DRAW_Z_ONLY(context, arena, s_hMyself);
		p->addDefaultComponents();
	}
};

struct MeshHelpers
{
	static PrimitiveTypes::UInt32 getNumberOfRangeCalls(IndexBufferGPU *pibGPU);

	static void analyzeTechniqueSequences(Mesh *pObj,
	PrimitiveTypes::UInt32 &numRanges,
	PrimitiveTypes::UInt32 &numFullSequences);

	static void pushEffects(Mesh *pObj);
	static void popEffects(Mesh *pObj);
	static void setPixelShadersOfTopEffects(PE::GameContext &context, PE::MemoryArena arena, Mesh *pObj);
	static void setEffectOfTopEffectSecuence(Mesh *pObj, Handle hNewEffect);
	static void setZOnlyEffectOfTopEffectSecuence(Mesh *pObj, Handle hNewEffect);
};

}; // namespace Components
}; // namespace PE
#endif
