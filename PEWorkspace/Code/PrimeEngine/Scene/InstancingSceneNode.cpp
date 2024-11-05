
#include "InstancingSceneNode.h"
#include "../Lua/LuaEnvironment.h"
#include "TextMesh.h"
#include "PrimeEngine/Events/StandardEvents.h"


namespace PE {
namespace Components {

PE_IMPLEMENT_CLASS1(InstancingSceneNode, SceneNode);

/*
// Constructor -------------------------------------------------------------
InstancingSceneNode::InstancingSceneNode(PE::GameContext &context, PE::MemoryArena arena, Handle hMyself)
: SceneNode(context, arena, hMyself)
{
}

void InstancingSceneNode::addDefaultComponents()
{
	SceneNode::addDefaultComponents();
	
	
	// event handlers
	//PE_REGISTER_EVENT_HANDLER(Events::Event_GATHER_DRAWCALLS, TextSceneNode::do_GATHER_DRAWCALLS);
}

void InstancingSceneNode::do_GATHER_DRAWCALLS(Events::Event *pEvt)
{
	Events::Event_GATHER_DRAWCALLS *pDrawEvent = NULL;
	pDrawEvent = (Events::Event_GATHER_DRAWCALLS *)(pEvt);

	Matrix4x4 projectionViewWorldMatrix = pDrawEvent->m_projectionViewTransform;
	Matrix4x4 worldMatrix;

	float numCharsInFullLine = 100.0f;
	numCharsInFullLine *= .5; // need to divide by 2.0 since screen goes from -1 to 1, not 0..1

	if (m_drawType == Overlay2D_3DPos)
	{
		worldMatrix = m_worldTransform;
#if APIABSTRACTION_OGL_INV
		projectionViewWorldMatrix = worldMatrix * projectionViewWorldMatrix;
#else
		projectionViewWorldMatrix = projectionViewWorldMatrix * worldMatrix;
#endif
}
*/
}; // namespace Components
}; // namespace PE
