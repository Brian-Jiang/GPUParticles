#ifndef __PrimeEngineNetworkManager_H__
#define __PrimeEngineNetworkManager_H__

// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

// Outer-Engine includes
#include <assert.h>
#include <map>

// Inter-Engine includes
#include "PrimeEngine/Utils/Networkable.h"

#include "../Events/Component.h"

extern "C"
{
#include "../../luasocket_dist/src/socket.h"
};

// Sibling/Children includes
#include "NetworkContext.h"

namespace PE {
namespace Components {

struct NetworkManager : public Component, public Networkable
{
	PE_DECLARE_CLASS(NetworkManager);
	PE_DECLARE_NETWORKABLE_CLASS

	// Constructor -------------------------------------------------------------
	NetworkManager(PE::GameContext &context, PE::MemoryArena arena, Handle hMyself);

	virtual ~NetworkManager();

	// Methods -----------------------------------------------------------------
	
	virtual void initNetwork();
	// Component ------------------------------------------------------------
	virtual void addDefaultComponents();

	void registerNetworkableObject(Networkable *pNetworkable);

	Networkable *getNetworkableObject(Networkable::NetworkId networkId);


	// is created per single connection
	virtual void createNetworkConnectionContext(t_socket sock, PE::NetworkContext *pNetContext);

	// Individual events -------------------------------------------------------
	PE_DECLARE_IMPLEMENT_EVENT_HANDLER_WRAPPER(do_UPDATE);
	virtual void do_UPDATE(Events::Event *pEvt);

	// Loading -----------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// Skin Lua Interface
	//////////////////////////////////////////////////////////////////////////
	//
	static void SetLuaFunctions(PE::Components::LuaEnvironment *pLuaEnv, lua_State *luaVM);
	//
	//static int l_GetSkin(lua_State *luaVM);
	//
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// Member variables 
	//////////////////////////////////////////////////////////////////////////
	
	typedef std::map<Networkable::NetworkId, Networkable *> NetworkableMap;
	NetworkableMap m_networkables;
};
}; // namespace Components
}; // namespace PE
#endif
