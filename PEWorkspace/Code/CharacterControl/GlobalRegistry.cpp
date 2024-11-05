#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

#include "GlobalRegistry.h"

#include "WayPoint.h"
#include "Characters/SoldierNPC.h"
#include "Characters/SoldierNPCAnimationSM.h"
#include "Characters/SoldierNPCMovementSM.h"
#include "Characters/SoldierNPCBehaviorSM.h"
#include "Characters/EnemyNPCMovementSM.h"
#include "Characters/TargetNPC.h"
#include "Events/Events.h"
#include "GameObjectMangerAddon.h"
#include "ClientGameObjectManagerAddon.h"
#include "ServerGameObjectManagerAddon.h"
#include "ClientCharacterControlGame.h"
#include "ServerCharacterControlGame.h"
#include "Tank/ClientTank.h"
#include "CharacterControl/Client/ClientSpaceShip.h"
#include "CharacterControl/Client/ClientSpaceShipControls.h"


using namespace PE::Components;
using namespace CharacterControl::Components;
using namespace CharacterControl::Events;
namespace CharacterControl
{
	bool setLuaMetaDataOnly = 0;
}

void CharacterControl::Register(PE::Components::LuaEnvironment *pLuaEnv, PE::GlobalRegistry *pRegistry)
{
	// register classes defined in CharacterControl

	pLuaEnv->StartRootRegistrationTable();
	// start root
	{
		pLuaEnv->StartRegistrationTable("CharacterControl");
		// start root.CharacterControl
		{
			pLuaEnv->StartRegistrationTable("Components");
			// start root.CharacterControl.Components
			{
				WayPoint::InitializeAndRegister(pLuaEnv, pRegistry, setLuaMetaDataOnly);
				SoldierNPC::InitializeAndRegister(pLuaEnv, pRegistry, setLuaMetaDataOnly);
				TargetNPC::InitializeAndRegister(pLuaEnv, pRegistry, setLuaMetaDataOnly);
				SoldierNPCAnimationSM::InitializeAndRegister(pLuaEnv, pRegistry, setLuaMetaDataOnly);
				SoldierNPCMovementSM::InitializeAndRegister(pLuaEnv, pRegistry, setLuaMetaDataOnly);
				SoldierNPCBehaviorSM::InitializeAndRegister(pLuaEnv, pRegistry, setLuaMetaDataOnly);
				EnemyNPCMovementSM::InitializeAndRegister(pLuaEnv, pRegistry, setLuaMetaDataOnly);
				TankController::InitializeAndRegister(pLuaEnv, pRegistry, setLuaMetaDataOnly);
				TankGameControls::InitializeAndRegister(pLuaEnv, pRegistry, setLuaMetaDataOnly);
				GameObjectManagerAddon::InitializeAndRegister(pLuaEnv, pRegistry, setLuaMetaDataOnly);
					ClientGameObjectManagerAddon::InitializeAndRegister(pLuaEnv, pRegistry, setLuaMetaDataOnly);
					ServerGameObjectManagerAddon::InitializeAndRegister(pLuaEnv, pRegistry, setLuaMetaDataOnly);
				ClientSpaceShip::InitializeAndRegister(pLuaEnv, pRegistry, setLuaMetaDataOnly);
				SpaceShipGameControls::InitializeAndRegister(pLuaEnv, pRegistry, setLuaMetaDataOnly);
			}
			// end root.CharacterControl.Components
			pLuaEnv->EndRegistrationTable();


			pLuaEnv->StartRegistrationTable("Events");
			// start root.CharacterControl.Events
			{
				Event_CreateSoldierNPC::InitializeAndRegister(pLuaEnv, pRegistry, setLuaMetaDataOnly);
				Event_CREATE_WAYPOINT::InitializeAndRegister(pLuaEnv, pRegistry, setLuaMetaDataOnly);
				SoldierNPCAnimSM_Event_STOP::InitializeAndRegister(pLuaEnv, pRegistry, setLuaMetaDataOnly);
				SoldierNPCAnimSM_Event_WALK::InitializeAndRegister(pLuaEnv, pRegistry, setLuaMetaDataOnly);
				SoldierNPCAnimSM_Event_Run::InitializeAndRegister(pLuaEnv, pRegistry, setLuaMetaDataOnly);
				SoldierNPCAnimSM_Event_StandShoot::InitializeAndRegister(pLuaEnv, pRegistry, setLuaMetaDataOnly);
				SoldierNPCMovementSM_Event_MOVE_TO::InitializeAndRegister(pLuaEnv, pRegistry, setLuaMetaDataOnly);
				SoldierNPCMovementSM_Event_TARGET_REACHED::InitializeAndRegister(pLuaEnv, pRegistry, setLuaMetaDataOnly);
				SoldierNPCMovementSM_Event_STOP::InitializeAndRegister(pLuaEnv, pRegistry, setLuaMetaDataOnly);
				SoldierNPCMovementSM_Event_Shoot::InitializeAndRegister(pLuaEnv, pRegistry, setLuaMetaDataOnly);
				Event_MoveTank_C_to_S::InitializeAndRegister(pLuaEnv, pRegistry, setLuaMetaDataOnly);
				Event_MoveTank_S_to_C::InitializeAndRegister(pLuaEnv, pRegistry, setLuaMetaDataOnly);
				Event_Tank_Throttle::InitializeAndRegister(pLuaEnv, pRegistry, setLuaMetaDataOnly);
				Event_Tank_Turn::InitializeAndRegister(pLuaEnv, pRegistry, setLuaMetaDataOnly);
			}
			// end root.CharacterControl.Events
			pLuaEnv->EndRegistrationTable();
		}
		// end root.CharacterControl
		pLuaEnv->EndRegistrationTable();
	}
	// end root
	pLuaEnv->EndRootRegistrationTable();

	setLuaMetaDataOnly = true; // make sure on next pass we dont reset class id, we just set registration values in lua
}

