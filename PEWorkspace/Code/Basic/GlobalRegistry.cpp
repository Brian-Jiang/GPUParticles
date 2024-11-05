#include "PrimeEngineIncludes.h"

#include "BasicIncludes.h"
#include "GlobalRegistry.h"
#include "ClientBasicGame.h"

namespace Basic
{
	bool setLuaMetaDataOnly = 0;
};

using namespace PE::Components;

void Basic::Register(PE::Components::LuaEnvironment *pLuaEnv, PE::GlobalRegistry *pRegistry)
{
	// register classes defined in Basic

	pLuaEnv->StartRootRegistrationTable();
	// start root
	{
		pLuaEnv->StartRegistrationTable("Basic");
		// start root.Basic
		{
			pLuaEnv->StartRegistrationTable("Components");
			// start root.Basic.Components
			{
				Basic::Components::TankController::InitializeAndRegister(pLuaEnv, pRegistry, setLuaMetaDataOnly);
			}
			// end root.Basic.Components
			pLuaEnv->EndRegistrationTable();


			pLuaEnv->StartRegistrationTable("Events");
			// start root.Basic.Events
			{
			}
			// end root.Basic.Events
			pLuaEnv->EndRegistrationTable();
		}
		// end root.Basic
		pLuaEnv->EndRegistrationTable();
	}
	// end root

	setLuaMetaDataOnly = true; // make sure on next pass we dont reset class id, we just set registration values in lua
}
