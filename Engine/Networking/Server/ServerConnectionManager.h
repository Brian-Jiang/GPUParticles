#ifndef __PrimeEngineServerConnectionManager_H__
#define __PrimeEngineServerConnectionManager_H__

// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

// Outer-Engine includes
#include <assert.h>

// Inter-Engine includes

#include "PrimeEngine/Events/Component.h"

// Sibling/Children includes

#include "PrimeEngine/Networking/ConnectionManager.h"

namespace PE {


namespace Components {


struct ServerConnectionManager : public ConnectionManager
{
	PE_DECLARE_CLASS(ServerConnectionManager);

	// Constructor -------------------------------------------------------------
	ServerConnectionManager(PE::GameContext &context, PE::MemoryArena arena, PE::NetworkContext &netContext, Handle hMyself);

	virtual ~ServerConnectionManager();

	// Methods -----------------------------------------------------------------
	virtual void initializeConnected(t_socket sock);

	// Component ------------------------------------------------------------
	virtual void addDefaultComponents();

	// Individual events -------------------------------------------------------
	PE_DECLARE_IMPLEMENT_EVENT_HANDLER_WRAPPER(do_UPDATE);
	virtual void do_UPDATE(Events::Event *pEvt);

	// Loading -----------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// Skin Lua Interface
	//////////////////////////////////////////////////////////////////////////
	//
	//static void SetLuaFunctions(PE::Components::LuaEnvironment *pLuaEnv, lua_State *luaVM);
	//
	//static int l_clientConnectToTCPServer(lua_State *luaVM);
	//
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// Member variables 
	//////////////////////////////////////////////////////////////////////////
	
};
}; // namespace Components
}; // namespace PE
#endif
