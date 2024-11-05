#define NOMINMAX
// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

#include "NetworkManager.h"

// Outer-Engine includes


// Inter-Engine includes
#include "../Lua/LuaEnvironment.h"

// Sibling/Children includes
#include "ConnectionManager.h"

// additional lua includes needed
extern "C"
{
#include "../../luasocket_dist/src/socket.h"
#include "../../luasocket_dist/src/inet.h"
};

#include "../../../GlobalConfig/GlobalConfig.h"
#include "PrimeEngine/Events/StandardEvents.h"

using namespace PE::Events;

namespace PE {
namespace Components {

PE_IMPLEMENT_CLASS1(NetworkManager, Component);

NetworkManager::NetworkManager(PE::GameContext &context, PE::MemoryArena arena, Handle hMyself)
: Component(context, arena, hMyself)
, Networkable(context, this) // don't register networkable trhough contructor since NetworkManager is nto constructed yet
{

	// can register networkable now here:
	m_networkId = s_NetworkId_NetworkManager;
	registerNetworkableObject(this);
}

NetworkManager::~NetworkManager()
{

}

void NetworkManager::addDefaultComponents()
{
	Component::addDefaultComponents();

	PE_REGISTER_EVENT_HANDLER(Events::Event_UPDATE, NetworkManager::do_UPDATE);
}

void NetworkManager::initNetwork()
{
	luasocket_localaddr(); // get ip(s) of local machine


}

void NetworkManager::registerNetworkableObject(Networkable *pNetworkable)
{
	assert(pNetworkable->m_networkId);

	NetworkableMap::iterator i = m_networkables.find(pNetworkable->m_networkId);
	assert(i == m_networkables.end());

	m_networkables[pNetworkable->m_networkId] = pNetworkable;

}

Networkable *NetworkManager::getNetworkableObject(Networkable::NetworkId networkId)
{
	assert(networkId);

	NetworkableMap::iterator i = m_networkables.find(networkId);
	assert(i != m_networkables.end());
	if (i != m_networkables.end())
		return i->second;

	return NULL;
}

void NetworkManager::createNetworkConnectionContext(t_socket sock, PE::NetworkContext *pNetContext)
{
}


void NetworkManager::do_UPDATE(Events::Event *pEvt)
{
	
}
//////////////////////////////////////////////////////////////////////////
// NetworkManager Lua Interface
//////////////////////////////////////////////////////////////////////////
//
void NetworkManager::SetLuaFunctions(PE::Components::LuaEnvironment *pLuaEnv, lua_State *luaVM)
{
/*
	static const struct luaL_Reg l_Skin[] = {
		{"l_GetSkin", l_GetSkin},
		{NULL, NULL} // sentinel
	};

	luaL_register(luaVM, 0, l_Skin);

	// run a script to add additional functionality to Lua side of Skin
	// that is accessible from Lua
#if APIABSTRACTION_IOS
	LuaEnvironment::Instance()->runScriptWorkspacePath("Code/PrimeEngine/Scene/Skin.lua");
#else
	LuaEnvironment::Instance()->runScriptWorkspacePath("Code\\PrimeEngine\\Scene\\Skin.lua");
#endif
*/
}
//
// retrieves debug information of the skin
/*
int NetworkManager::l_GetSkin(lua_State *luaVM)
{
	Handle h;
	LuaGlue::popHandleFromTableOnStackAndPopTable(luaVM, h);
	Skin *pSkin = h.getObject<Skin>();

	// table that will be returned
	lua_newtable(luaVM);

	lua_pushstring(luaVM, "animNames"); // 
	lua_newtable(luaVM); // list of names
	{
		AnimNameSet *pNameSet = pSkin->m_hNameSet.getObject<AnimNameSet>();
		for (int i = 0; i < (int)(pNameSet->m_size); i++)
		{
			lua_pushnumber(luaVM, i+1); // push index of the name
			lua_pushstring(luaVM, pNameSet->m_strings[i].getCStrPtr());
			lua_rawset(luaVM, -3); // set result["animNames"][i+1]
		}
	}
	lua_rawset(luaVM, -3); // set list of names

	return 1; // the result is the table on the stack
}
*/
//////////////////////////////////////////////////////////////////////////

	
}; // namespace Components
}; // namespace PE
