#ifndef __PrimeEngineClientNetworkManager_H__
#define __PrimeEngineClientNetworkManager_H__

// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

// Outer-Engine includes
#include <assert.h>

// Inter-Engine includes

#include "PrimeEngine/Events/Component.h"

// Sibling/Children includes

#include "PrimeEngine/Networking/NetworkManager.h"

namespace PE {
namespace Components {

struct ClientNetworkManager : public NetworkManager
{
	PE_DECLARE_CLASS(ClientNetworkManager);

	// Constructor -------------------------------------------------------------
	ClientNetworkManager(PE::GameContext &context, PE::MemoryArena arena, Handle hMyself);

	virtual ~ClientNetworkManager();

	enum EClientState
	{
		ClientState_Disconnected = 0,
		ClientState_Connecting,
		ClientState_Connected,
		ClientState_Count
	};

	// accessors
	NetworkContext &getNetworkContext(){return m_netContext;}

	// Methods -----------------------------------------------------------------
	virtual void initNetwork();

	const char *EClientStateToString(EClientState state);

	void debugRender(int &threadOwnershipMask, float xoffset = 0, float yoffset = 0);


	void clientConnectToTCPServer(const char *strAddr, int port);

	virtual void createNetworkConnectionContext(t_socket sock, PE::NetworkContext *pNetContext);

	// Component ------------------------------------------------------------
	virtual void addDefaultComponents();

	// Individual events -------------------------------------------------------

	PE_DECLARE_IMPLEMENT_EVENT_HANDLER_WRAPPER(do_SERVER_CLIENT_CONNECTION_ACK);
	virtual void do_SERVER_CLIENT_CONNECTION_ACK(PE::Events::Event *pEvt);

	PE_DECLARE_IMPLEMENT_EVENT_HANDLER_WRAPPER(do_UPDATE);
	virtual void do_UPDATE(Events::Event *pEvt);


	// Loading -----------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// Skin Lua Interface
	//////////////////////////////////////////////////////////////////////////
	//
	static void SetLuaFunctions(PE::Components::LuaEnvironment *pLuaEnv, lua_State *luaVM);
	//
	static int l_clientConnectToTCPServer(lua_State *luaVM);
	//
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// Member variables 
	//////////////////////////////////////////////////////////////////////////
	EClientState m_state;

	NetworkContext m_netContext; // Client apps have only one context
	Threading::Mutex m_netContextLock;

	int m_clientId;
};
}; // namespace Components
}; // namespace PE
#endif
