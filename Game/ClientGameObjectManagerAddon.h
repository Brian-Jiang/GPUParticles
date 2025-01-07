#ifndef _CHARACTER_CONTROL_CLIENT_GAME_OBJ_MANAGER_ADDON_
#define _CHARACTER_CONTROL_CLIENT_GAME_OBJ_MANAGER_ADDON_

#include "GameObjectMangerAddon.h"
#include "Events/Events.h"

#include "WayPoint.h"
#include "Characters/TargetNPC.h"

namespace CharacterControl
{
namespace Components
{

// This struct will be added to GameObjectManager as component
// as a result events sent to game object manager will be able to get to this component
// so we can create custom game objects through this class
struct ClientGameObjectManagerAddon : public GameObjectManagerAddon
{
	PE_DECLARE_CLASS(ClientGameObjectManagerAddon); // creates a static handle and GteInstance*() methods. still need to create construct

	ClientGameObjectManagerAddon(PE::GameContext &context, PE::MemoryArena arena, PE::Handle hMyself) : GameObjectManagerAddon(context, arena, hMyself)
	{}

	// sub-component and event registration
	virtual void addDefaultComponents() ;

	PE_DECLARE_IMPLEMENT_EVENT_HANDLER_WRAPPER(do_CreateSoldierNPC);
	virtual void do_CreateSoldierNPC(PE::Events::Event *pEvt);

	PE_DECLARE_IMPLEMENT_EVENT_HANDLER_WRAPPER(do_CREATE_WAYPOINT);
	virtual void do_CREATE_WAYPOINT(PE::Events::Event *pEvt);

	//will activate tank when local client is connected
	PE_DECLARE_IMPLEMENT_EVENT_HANDLER_WRAPPER(do_SERVER_CLIENT_CONNECTION_ACK);
	virtual void do_SERVER_CLIENT_CONNECTION_ACK(PE::Events::Event *pEvt);

	// sent from server, sets position of non-local client tanks
	PE_DECLARE_IMPLEMENT_EVENT_HANDLER_WRAPPER(do_MoveTank);
	virtual void do_MoveTank(PE::Events::Event *pEvt);


	// no need to implement this as eent since tank creation will be hardcoded
	void createTank(int index, int &threadOwnershipMask);

	void createSpaceShip(int &threadOwnershipMask);
	void createSoldierNPC(Vector3 pos, int &threadOwnershipMask);
	void createSoldierNPC(Events::Event_CreateSoldierNPC *pTrueEvent);
	void createTargetNPC(Vector3 pos, int& threadOwnershipMask);
	void createTargetNPC(Events::Event_CreateTargetNPC *pTrueEvent);


	//////////////////////////////////////////////////////////////////////////
	// Game Specific functionality
	//////////////////////////////////////////////////////////////////////////
	//
	// waypoint search
	WayPoint *getWayPoint(const char *name);

	TargetNPC *getTargetNPC();
};


}
}

#endif
