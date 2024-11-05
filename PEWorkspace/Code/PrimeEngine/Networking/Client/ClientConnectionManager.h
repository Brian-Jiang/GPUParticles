#ifndef __PrimeEngineClientConnectionManager_H__
#define __PrimeEngineClientConnectionManager_H__

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

struct ClientConnectionManager : public ConnectionManager
{

	PE_DECLARE_CLASS(ClientConnectionManager);

	// Constructor -------------------------------------------------------------
	ClientConnectionManager(PE::GameContext &context, PE::MemoryArena arena, PE::NetworkContext &netContext, Handle hMyself);

	virtual ~ClientConnectionManager();


	// Methods -----------------------------------------------------------------
	virtual void initializeConnected(t_socket sock);

	
	// Component ------------------------------------------------------------
	virtual void addDefaultComponents();

	// Individual events -------------------------------------------------------

	// Loading -----------------------------------------------------------------

#if 0 // Template
	//////////////////////////////////////////////////////////////////////////
	// Skin Lua Interface
	//////////////////////////////////////////////////////////////////////////
	//
	static void SetLuaFunctions(PE::Components::LuaEnvironment *pLuaEnv, lua_State *luaVM);
	//
	static int l_clientConnectToTCPServer(lua_State *luaVM);
	//
	//////////////////////////////////////////////////////////////////////////
#endif
	//////////////////////////////////////////////////////////////////////////
	// Member variables 
	//////////////////////////////////////////////////////////////////////////
};
}; // namespace Components
}; // namespace PE
#endif
