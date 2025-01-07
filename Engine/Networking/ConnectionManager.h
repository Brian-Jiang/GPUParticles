#ifndef __PrimeEngineConnectionManager_H__
#define __PrimeEngineConnectionManager_H__

// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

// Outer-Engine includes
#include <assert.h>
#include <vector>

// Inter-Engine includes

#include "../Events/Component.h"
#include "../../../GlobalConfig/GlobalConfig.h"

extern "C"
{
#include "../../luasocket_dist/src/socket.h"
};


// Sibling/Children includes
#include "PrimeEngine/Networking/NetworkContext.h"
#include "Packet.h"

namespace PE {
namespace Components {

struct ConnectionManager : public Component
{
	PE_DECLARE_CLASS(ConnectionManager);

	// Constructor -------------------------------------------------------------
	ConnectionManager(PE::GameContext &context, PE::MemoryArena arena, PE::NetworkContext &netContext, Handle hMyself);

	virtual ~ConnectionManager();

	// note: there is no connecting state. connecting is done by network mananger
	// once this connection manager disconnects, it never reconnects on its own

	enum EConnectionManagerState
	{
		ConnectionManagerState_Disconnected = 0,
		ConnectionManagerState_Connected,
		ConnectionManagerState_Count
	};

	// Methods -----------------------------------------------------------------
	virtual void initializeConnected(t_socket sock);

	void sendPacket(Packet *pPacket, TransmissionRecord *pTransmissionRecord);
	
	void receivePackets();
	bool connected() const {return m_state == ConnectionManagerState_Connected;}
	void disconnect();

	// Component ------------------------------------------------------------
	virtual void addDefaultComponents();

	// Individual events -------------------------------------------------------
	PE_DECLARE_IMPLEMENT_EVENT_HANDLER_WRAPPER(do_UPDATE);
	virtual void do_UPDATE(Events::Event *pEvt);

	// Loading -----------------------------------------------------------------

#if 0 // template
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
	struct AckSim
	{
		bool m_delivered;
		int m_packetId;
	};
	std::vector<AckSim> m_ackSimulation;

	PE::NetworkContext *m_pNetContext;

	/*luasocket::*/t_socket m_sock; // tcp connection socket
	EConnectionManagerState m_state;

	int m_bytesNeededForNextPacket;

	int m_bytesBuffered;
	char m_buffer[PE_SOCKET_RECEIVE_BUFFER_SIZE];

};
}; // namespace Components
}; // namespace PE
#endif
