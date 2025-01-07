#ifndef __PYENGINE_2_0_GAMEOBJECTMANAGER_H__
#define __PYENGINE_2_0_GAMEOBJECTMANAGER_H__

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
#include "../Scene/RootSceneNode.h"
#include "../Scene/CameraManager.h"

#include "PrimeEngine/Utils/Networkable.h"

// Sibling/Children includes
namespace PE {
namespace Components{

struct GameObjectManager : public Component, public Networkable
{
	PE_DECLARE_CLASS(GameObjectManager);
	PE_DECLARE_NETWORKABLE_CLASS

	// Singleton ------------------------------------------------------------------

	// Constructor -------------------------------------------------------------
	GameObjectManager(PE::GameContext &context, PE::MemoryArena arena, Handle hMyself);
	virtual ~GameObjectManager(){}
	// Methods      ------------------------------------------------------------
	PE_DECLARE_IMPLEMENT_EVENT_HANDLER_WRAPPER(do_SET_DEBUG_TARGET_HANDLE);
	void do_SET_DEBUG_TARGET_HANDLE(Events::Event *pEvt);

	PE_DECLARE_IMPLEMENT_EVENT_HANDLER_WRAPPER(do_CREATE_LIGHT);
	void do_CREATE_LIGHT(Events::Event *pEvt);

	PE_DECLARE_IMPLEMENT_EVENT_HANDLER_WRAPPER(do_CREATE_SKELETON);
	void do_CREATE_SKELETON(Events::Event *pEvt);

	PE_DECLARE_IMPLEMENT_EVENT_HANDLER_WRAPPER(do_CREATE_ANIM_SET);
	void do_CREATE_ANIM_SET(Events::Event *pEvt);

	PE_DECLARE_IMPLEMENT_EVENT_HANDLER_WRAPPER(do_CREATE_MESH);
	void do_CREATE_MESH(Events::Event *pEvt);

	PE_DECLARE_IMPLEMENT_EVENT_HANDLER_WRAPPER(do_CONSTRUCT_SOUND);
	void do_CONSTRUCT_SOUND(Events::Event *pEvt);

	PE_DECLARE_IMPLEMENT_EVENT_HANDLER_WRAPPER(do_CREATE_HEAD);
	void do_CREATE_HEAD(Events::Event *pEvt);

	PE_DECLARE_IMPLEMENT_EVENT_HANDLER_WRAPPER(do_CREATE_PS3PSYS);
	void do_CREATE_PS3PSYS(Events::Event *pEvt);

	// Component ------------------------------------------------------------

	virtual void addDefaultComponents();
	// Individual events -------------------------------------------------------
	
	Handle m_lastAddedObjHandle;
	Handle m_lastAddedSkelInstanceHandle;

	private:
		LuaGlue::LuaReference m_luaGameObjectTableRef;

	void createGameObjectTableIfDoesntExist();
	void putGameObjectTableIOnStack();
};

}; // namespace Components
}; // namespace PE
#endif
