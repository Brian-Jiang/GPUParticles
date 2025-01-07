#ifndef __PYENGINE_2_0_ROOTSCENENODE_H__
#define __PYENGINE_2_0_ROOTSCENENODE_H__

// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

// Outer-Engine includes
#include <assert.h>

// Inter-Engine includes
#include "PrimeEngine/MemoryManagement/Handle.h"
#include "PrimeEngine/PrimitiveTypes/PrimitiveTypes.h"
#include "../Events/Component.h"
#include "../Utils/Array/Array.h"
#include "PrimeEngine/APIAbstraction/Effect/Effect.h"


// Sibling/Children includes
#include "SceneNode.h"

namespace PE{
namespace Components {

struct RootSceneNode : public SceneNode
{
	PE_DECLARE_CLASS(RootSceneNode);

	static void Construct(PE::GameContext &context, PE::MemoryArena arena);

	// Constructor -------------------------------------------------------------
	// same
	RootSceneNode(PE::GameContext &context, PE::MemoryArena arena, Handle hMyself) : SceneNode(context, arena, hMyself)
	{
		m_components.reset(512);
	}

	virtual ~RootSceneNode(){}

	// component
	virtual void addDefaultComponents();
	// Individual events -------------------------------------------------------
	// this method will set up some global gpu constants like game time, frame time
	// it will also set light source gpu constants
	PE_DECLARE_IMPLEMENT_EVENT_HANDLER_WRAPPER(do_GATHER_DRAWCALLS);
	virtual void do_GATHER_DRAWCALLS(Events::Event *pEvt);

	static RootSceneNode *Instance() {return s_hInstance.getObject<RootSceneNode>();}
	static RootSceneNode *TitleInstance() {return s_hTitleInstance.getObject<RootSceneNode>();}
	static Handle InstanceHandle() {return s_hInstance;}
	static Handle TitleInstanceHandle() {return s_hTitleInstance;}
	static RootSceneNode *CurInstance() {return s_hCurInstance.getObject<RootSceneNode>();}
	static Handle CurInstanceHandle() {return s_hCurInstance;}
	static void SetTitleAsCurrent(){ s_hCurInstance = s_hTitleInstance; }
	static void SetGameAsCurrent() { s_hCurInstance = s_hInstance; }
	static bool TitleIsCurrent() { return s_hCurInstance == s_hTitleInstance;}

	static void SetInstance(Handle h){s_hInstance = h;}
	private:
		static Handle s_hInstance;
		static Handle s_hTitleInstance;
		static Handle s_hCurInstance;

};

}; // namespace Components
}; // namespace PE

#endif // file guard
