#define NOMINMAX
// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

#include "ClientNetworkManager.h"

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

#include "PrimeEngine/Scene/DebugRenderer.h"
#include "PrimeEngine/Events/StandardEvents.h"

#include "PrimeEngine/Networking/Client/ClientConnectionManager.h"

#include "PrimeEngine/Networking/StreamManager.h"
#include "PrimeEngine/Networking/EventManager.h"

// Sibling/Children includes
using namespace PE::Events;

namespace PE {
namespace Components {

PE_IMPLEMENT_CLASS1(ClientNetworkManager, NetworkManager);

ClientNetworkManager::ClientNetworkManager(PE::GameContext &context, PE::MemoryArena arena, Handle hMyself)
: NetworkManager(context, arena, hMyself)
, m_clientId(-1)
{
	m_state = ClientState_Disconnected;
}

ClientNetworkManager::~ClientNetworkManager()
{

}

void ClientNetworkManager::initNetwork()
{
	NetworkManager::initNetwork();
}

void ClientNetworkManager::clientConnectToTCPServer(const char *strAddr, int port)
{
	bool created = false;
	int numTries = 0;

	if (port == 0)
		port = PE_SERVER_PORT;

	t_socket sock;
	while (!created)
	{
		const char *err = /*luasocket::*/inet_trycreate(&sock, SOCK_STREAM);
		if (err)
		{
			assert(!"error creating socket occurred");
			break;
		}

		t_timeout timeout; // timeout supports managind timeouts of multiple blocking alls by using total.
		// but if total is < 0 it just uses block value for each blocking call
		timeout.block = PE_CLIENT_TO_SERVER_CONNECT_TIMEOUT;
		timeout.total = -1.0;
		timeout.start = 0;

		err = inet_tryconnect(&sock, strAddr, port, &timeout);

		numTries++;
		if (err) {
			PEINFO("PE: Warning: Failed to connect to %s:%d reason: %s\n", strAddr, port, err);
			
			if (numTries >= 10)
			{
				break; // give up
			}
		}
		else
		{
			PEINFO("PE: Client connected to %s:%d\n", strAddr, port);
			created = true;
			m_netContextLock.lock();
			m_state = ClientState_Connected;
			
			createNetworkConnectionContext(sock, &m_netContext);
			m_netContextLock.unlock();
			break;
		}
	}
}

void ClientNetworkManager::createNetworkConnectionContext(t_socket sock, PE::NetworkContext *pNetContext)
{
	NetworkManager::createNetworkConnectionContext(sock, pNetContext);

	{
		pNetContext->m_pConnectionManager = new (m_arena) ClientConnectionManager(*m_pContext, m_arena, *pNetContext, Handle());
		pNetContext->getConnectionManager()->addDefaultComponents();
	}

	{
		pNetContext->m_pStreamManager = new (m_arena) StreamManager(*m_pContext, m_arena, *pNetContext, Handle());
		pNetContext->getStreamManager()->addDefaultComponents();
	}

	{
		pNetContext->m_pEventManager = new (m_arena) EventManager(*m_pContext, m_arena, *pNetContext, Handle());
		pNetContext->getEventManager()->addDefaultComponents();
	}

	pNetContext->getConnectionManager()->initializeConnected(sock);

	addComponent(pNetContext->getConnectionManager()->getHandle());
	addComponent(pNetContext->getStreamManager()->getHandle());
}

void ClientNetworkManager::addDefaultComponents()
{
	NetworkManager::addDefaultComponents();

	// no need to register handler as parent class already has this method registered
	// PE_REGISTER_EVENT_HANDLER(Events::Event_UPDATE, ServerConnectionManager::do_UPDATE);

	PE_REGISTER_EVENT_HANDLER(PE::Events::Event_SERVER_CLIENT_CONNECTION_ACK, ClientNetworkManager::do_SERVER_CLIENT_CONNECTION_ACK);
}

void ClientNetworkManager::do_UPDATE(PE::Events::Event *pEvt)
{
	NetworkManager::do_UPDATE(pEvt);
	if (m_netContext.getConnectionManager())
	{
		if (m_state == ClientState_Connected)
		{
			if (!m_netContext.getConnectionManager()->connected())
			{
				// disconnect happened
				m_state = ClientState_Disconnected;
				m_clientId = -1;
			}
		}
	}
}

void ClientNetworkManager::do_SERVER_CLIENT_CONNECTION_ACK(PE::Events::Event *pEvt)
{
	Event_SERVER_CLIENT_CONNECTION_ACK *pRealEvt = (Event_SERVER_CLIENT_CONNECTION_ACK *)(pEvt);
	m_clientId = pRealEvt->m_clientId;
}

const char *ClientNetworkManager::EClientStateToString(EClientState state)
{
	switch(state)
	{
	case ClientState_Disconnected : return "ClientState_Disconnected";
	case ClientState_Connecting : return "ClientState_Connecting";
	case ClientState_Connected : return "ClientState_Connected";
	default: return "Unknown State";
	};
}

void ClientNetworkManager::debugRender(int &threadOwnershipMask, float xoffset /* = 0*/, float yoffset /* = 0*/)
{
	sprintf(PEString::s_buf, "Client: %s Id: %d", EClientStateToString(m_state), m_clientId);
	DebugRenderer::Instance()->createTextMesh(
		PEString::s_buf, true, false, false, false, 0,
		Vector3(xoffset, yoffset, 0), 1.0f, threadOwnershipMask);

	if (m_clientId == -1)
		return;

	float dy = 0.025f;
	float dx = 0.01;

	m_netContextLock.lock();
	
	m_netContext.getEventManager()->debugRender(threadOwnershipMask, xoffset + dx, yoffset + dy);
	
	m_netContextLock.unlock();
}

//////////////////////////////////////////////////////////////////////////
// ConnectionManager Lua Interface
//////////////////////////////////////////////////////////////////////////
//
void ClientNetworkManager::SetLuaFunctions(PE::Components::LuaEnvironment *pLuaEnv, lua_State *luaVM)
{
	/*
	static const struct luaL_Reg l_functions[] = {
	{"l_clientConnectToTCPServer", l_clientConnectToTCPServer},
	{NULL, NULL} // sentinel
	};
	luaL_register(luaVM, 0, l_functions);

	VS

	lua_register(luaVM, "l_clientConnectToTCPServer", l_clientConnectToTCPServer);

	Note: registering with luaL_register is preferred since it is putting functions into
	table associated with this class
	using lua_register puts functions into global space
	*/
	static const struct luaL_Reg l_functions[] = {
		{"l_clientConnectToTCPServer", l_clientConnectToTCPServer},
		{NULL, NULL} // sentinel
	};

	luaL_register(luaVM, 0, l_functions);

	// run a script to add additional functionality to Lua side of the class
	// that is accessible from Lua
// #if APIABSTRACTION_IOS
// 	LuaEnvironment::Instance()->runScriptWorkspacePath("Code/PrimeEngine/Scene/Skin.lua");
// #else
// 	LuaEnvironment::Instance()->runScriptWorkspacePath("Code\\PrimeEngine\\Scene\\Skin.lua");
// #endif

}

int ClientNetworkManager::l_clientConnectToTCPServer(lua_State *luaVM)
{
	lua_Number lPort = lua_tonumber(luaVM, -1);
	int port = (int)(lPort);

	const char *strAddr = lua_tostring(luaVM, -2);

	GameContext *pContext = (GameContext *)(lua_touserdata(luaVM, -3));

	lua_pop(luaVM, 3);

	ClientNetworkManager *pClientNetwManager = (ClientNetworkManager *)(pContext->getNetworkManager());
	pClientNetwManager->clientConnectToTCPServer(strAddr, port);

	return 0; // no return values
}

}; // namespace Components
}; // namespace PE
