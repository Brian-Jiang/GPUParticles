#ifndef _CHARACTER_CONTROL_GAME_OBJ_MANAGER_ADDON_
#define _CHARACTER_CONTROL_GAME_OBJ_MANAGER_ADDON_

#include "PrimeEngine/Events/Component.h"
#include "Events/Events.h"

#include "WayPoint.h"

namespace CharacterControl
{
namespace Components
{

// This struct will be added to GameObjectManager as component
// as a result events sent to game object manager will be able to get to this component
// so we can create custom game objects through this class
struct GameObjectManagerAddon : public PE::Components::Component
{
	PE_DECLARE_CLASS(GameObjectManagerAddon); // creates a static handle and GteInstance*() methods. still need to create construct

	GameObjectManagerAddon(PE::GameContext &context, PE::MemoryArena arena, PE::Handle hMyself) : Component(context, arena, hMyself)
	{}

	// sub-component and event registration
	virtual void addDefaultComponents() ;

	//////////////////////////////////////////////////////////////////////////
	// Game Specific functionality
	//////////////////////////////////////////////////////////////////////////
	//
};


}
}

#endif
