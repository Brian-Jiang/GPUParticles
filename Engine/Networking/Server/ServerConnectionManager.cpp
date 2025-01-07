#define NOMINMAX
// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

#include "ServerConnectionManager.h"

// Outer-Engine includes

// Inter-Engine includes

#include "PrimeEngine/Lua/LuaEnvironment.h"

// additional lua includes needed
extern "C"
{
#include "PrimeEngine/../luasocket_dist/src/socket.h"
#include "PrimeEngine/../luasocket_dist/src/inet.h"
};

#include "PrimeEngine/../../GlobalConfig/GlobalConfig.h"

#include "PrimeEngine/Events/StandardEvents.h"

#include "PrimeEngine/Scene/DebugRenderer.h"

// Sibling/Children includes
using namespace PE::Events;

namespace PE {


namespace Components {

PE_IMPLEMENT_CLASS1(ServerConnectionManager, ConnectionManager);

ServerConnectionManager::ServerConnectionManager(PE::GameContext &context, PE::MemoryArena arena, PE::NetworkContext &netContext, Handle hMyself)
: ConnectionManager(context, arena, netContext, hMyself)
{
	
}

ServerConnectionManager::~ServerConnectionManager()
{

}

void ServerConnectionManager::initializeConnected(t_socket sock)
{
	ConnectionManager::initializeConnected(sock);
}

void ServerConnectionManager::addDefaultComponents()
{
	ConnectionManager::addDefaultComponents();

	// no need to register handler as parent class already has this method registered
	// PE_REGISTER_EVENT_HANDLER(Events::Event_UPDATE, ServerConnectionManager::do_UPDATE);
}

void ServerConnectionManager::do_UPDATE(Events::Event *pEvt)
{
	ConnectionManager::do_UPDATE(pEvt);
}

#if 0 // template
//////////////////////////////////////////////////////////////////////////
// ConnectionManager Lua Interface
//////////////////////////////////////////////////////////////////////////
//
void ConnectionManager::SetLuaFunctions(PE::Components::LuaEnvironment *pLuaEnv, lua_State *luaVM)
{
	
	//static const struct luaL_Reg l_functions[] = {
	//	{"l_clientConnectToTCPServer", l_clientConnectToTCPServer},
	//	{NULL, NULL} // sentinel
	//};

	//luaL_register(luaVM, 0, l_functions);
	
	lua_register(luaVM, "l_clientConnectToTCPServer", l_clientConnectToTCPServer);


	// run a script to add additional functionality to Lua side of Skin
	// that is accessible from Lua
// #if APIABSTRACTION_IOS
// 	LuaEnvironment::Instance()->runScriptWorkspacePath("Code/PrimeEngine/Scene/Skin.lua");
// #else
// 	LuaEnvironment::Instance()->runScriptWorkspacePath("Code\\PrimeEngine\\Scene\\Skin.lua");
// #endif

}

int ConnectionManager::l_clientConnectToTCPServer(lua_State *luaVM)
{
	lua_Number lPort = lua_tonumber(luaVM, -1);
	int port = (int)(lPort);

	const char *strAddr = lua_tostring(luaVM, -2);

	GameContext *pContext = (GameContext *)(lua_touserdata(luaVM, -3));

	lua_pop(luaVM, 3);

	pContext->getConnectionManager()->clientConnectToTCPServer(strAddr, port);

	return 0; // no return values
}

//////////////////////////////////////////////////////////////////////////
#endif
	
}; // namespace Components
}; // namespace PE
