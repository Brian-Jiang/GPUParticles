#define NOMINMAX
// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

#include "ServerNetworkManager.h"

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

#include "PrimeEngine/GameObjectModel/GameObjectManager.h"
#include "PrimeEngine/Events/StandardEvents.h"
#include "PrimeEngine/Scene/DebugRenderer.h"

#include "PrimeEngine/Networking/StreamManager.h"
#include "PrimeEngine/Networking/EventManager.h"

// Sibling/Children includes
#include "ServerConnectionManager.h"

using namespace PE::Events;

namespace PE {

namespace Components {

PE_IMPLEMENT_CLASS1(ServerNetworkManager, NetworkManager);

ServerNetworkManager::ServerNetworkManager(PE::GameContext &context, PE::MemoryArena arena, Handle hMyself)
: NetworkManager(context, arena, hMyself)
, m_clientConnections(context, arena, PE_SERVER_MAX_CONNECTIONS)
{
	m_state = ServerState_Uninitialized;
}

ServerNetworkManager::~ServerNetworkManager()
{
	if (m_state != ServerState_Uninitialized)
		socket_destroy(&m_sock);
}

void ServerNetworkManager::addDefaultComponents()
{
	NetworkManager::addDefaultComponents();

	// no need to register handler as parent class already has this method registered
	// PE_REGISTER_EVENT_HANDLER(Events::Event_UPDATE, ServerConnectionManager::do_UPDATE);
}

void ServerNetworkManager::initNetwork()
{
	NetworkManager::initNetwork();

	serverOpenTCPSocket();
}

void ServerNetworkManager::serverOpenTCPSocket()
{
	bool created = false;
	int numTries = 0;
	int port = PE_SERVER_PORT;
	
	while (!created)
	{
		const char *err = /*luasocket::*/inet_trycreate(&m_sock, SOCK_STREAM);
		if (err)
		{
			assert(!"error creating socket occurred");
			break;
		}

		err = inet_trybind(&m_sock, "0.0.0.0", (unsigned short)(port)); // leaves socket non-blocking
		numTries++;
		if (err) {
			if (numTries >= 10)
				break; // give up
			port++;
		}
		else
		{
			created = true;
			break;
		}
	}

	if (created)
		m_serverPort = port;
	else
	{
		assert(!"Could not create server");
		return;
	}

	const char *err = inet_trylisten(&m_sock, PE_SERVER_MAX_CONNECTIONS); // leaves socket non-blocking
	if (err)
	{
		PEINFO("Warning: Could not listen on socket. Err: %s\n", err);
		assert(!"Could not listen on socket");
		return;
	}

	m_state = ServerState_ConnectionListening;
}


void ServerNetworkManager::createNetworkConnectionContext(t_socket sock,  int clientId, PE::NetworkContext *pNetContext)
{
	
	pNetContext->m_clientId = clientId;

	NetworkManager::createNetworkConnectionContext(sock, pNetContext);

	{
		pNetContext->m_pConnectionManager = new (m_arena) ServerConnectionManager(*m_pContext, m_arena, *pNetContext, Handle());
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



void ServerNetworkManager::do_UPDATE(Events::Event *pEvt)
{
	NetworkManager::do_UPDATE(pEvt);

	t_timeout timeout; // timeout supports managing timeouts of multiple blocking alls by using total.
	// but if total is < 0 it just uses block value for each blocking call
	timeout.block = 0;
	timeout.total = -1.0;
	timeout.start = 0;

	t_socket sock;
	int err = socket_accept(&m_sock, &sock, NULL, NULL, &timeout);
	if (err != IO_DONE)
	{
		const char *s = socket_strerror(err);
		return;
	}

	if (err == IO_DONE)
	{
		m_connectionsMutex.lock();
		m_clientConnections.add(NetworkContext());
		int clientIndex = m_clientConnections.m_size-1;
		NetworkContext &netContext = m_clientConnections[clientIndex];

		// create a tribes stack for this connection
		createNetworkConnectionContext(sock, clientIndex, &netContext);
		m_connectionsMutex.unlock();

		PE::Events::Event_SERVER_CLIENT_CONNECTION_ACK evt(*m_pContext);
		evt.m_clientId = clientIndex;

		netContext.getEventManager()->scheduleEvent(&evt, m_pContext->getGameObjectManager(), true);

	}
}

void ServerNetworkManager::debugRender(int &threadOwnershipMask, float xoffset /* = 0*/, float yoffset /* = 0*/)
{
	sprintf(PEString::s_buf, "Server: Port %d %d Connections", m_serverPort, m_clientConnections.m_size);
	DebugRenderer::Instance()->createTextMesh(
		PEString::s_buf, true, false, false, false, 0,
		Vector3(xoffset, yoffset, 0), 1.0f, threadOwnershipMask);

	float dy = 0.025f;
	float dx = 0.01;
	float evtManagerDy = 0.15f;
	// debug render all networking contexts
	m_connectionsMutex.lock();
	for (unsigned int i = 0; i < m_clientConnections.m_size; ++i)
	{
		sprintf(PEString::s_buf, "Connection[%d]:", i);
	
		DebugRenderer::Instance()->createTextMesh(
		PEString::s_buf, true, false, false, false, 0,
		Vector3(xoffset, yoffset + dy + evtManagerDy * i, 0), 1.0f, threadOwnershipMask);

		NetworkContext &netContext = m_clientConnections[i];
		netContext.getEventManager()->debugRender(threadOwnershipMask, xoffset + dx, yoffset + dy * 2.0f + evtManagerDy * i);
	}
	m_connectionsMutex.unlock();
}

void ServerNetworkManager::scheduleEventToAllExcept(PE::Networkable *pNetworkable, PE::Networkable *pNetworkableTarget, int exceptClient)
{
	for (unsigned int i = 0; i < m_clientConnections.m_size; ++i)
	{
		if ((int)(i) == exceptClient)
			continue;

		NetworkContext &netContext = m_clientConnections[i];
		netContext.getEventManager()->scheduleEvent(pNetworkable, pNetworkableTarget, true);
	}
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
