#ifndef __PrimeEngineEventManager_H__
#define __PrimeEngineEventManager_H__

// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

// Outer-Engine includes
#include <assert.h>
#include <vector>
#include <deque>

// Inter-Engine includes

#include "../Events/Component.h"

extern "C"
{
#include "../../luasocket_dist/src/socket.h"
};

#include "PrimeEngine/Networking/NetworkContext.h"
#include "PrimeEngine/Utils/Networkable.h"

// Sibling/Children includes
#include "Packet.h"

namespace PE {
namespace Components {

struct EventManager : public Component
{
	static const int PE_EVENT_SLIDING_WINDOW = 64;

	PE_DECLARE_CLASS(EventManager);

	// Constructor -------------------------------------------------------------
	EventManager(PE::GameContext &context, PE::MemoryArena arena, PE::NetworkContext &netContext, Handle hMyself);

	virtual ~EventManager();

	// Methods -----------------------------------------------------------------
	virtual void initialize();

	/// called by gameplay code to schedule event transmission to client(s)
	void scheduleEvent(PE::Networkable *pNetworkable, PE::Networkable *pNetworkableTarget, bool guaranteed);

	/// called by stream manager to see how many events to send
	int haveEventsToSend();

	/// called by StreamManager to put queued up events in packet
	int fillInNextPacket(char *pDataStream, TransmissionRecord *pRecord, int packetSizeAllocated, bool &out_usefulDataSent, bool &out_wantToSendMore);

	/// called by StreamManager to process transmission record deliver notification
	void processNotification(TransmissionRecord *pTransmittionRecord, bool delivered);

	void debugRender(int &threadOwnershipMask, float xoffset = 0, float yoffset = 0);

	int receiveNextPacket(char *pDataStream);
	
	// Component ------------------------------------------------------------
	virtual void addDefaultComponents();

	// Individual events -------------------------------------------------------

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

	std::deque<EventTransmissionData> m_eventsToSend;

	// transmitter
	int m_transmitterNextEvtOrderId;
	int m_transmitterNumEventsNotAcked; //= number of events stored in TransmissionRecords


	// receiver
	int m_receiverFirstEvtOrderId; // evtOrderId of first element in m_receivedEvents

	EventReceptionData m_receivedEvents[PE_EVENT_SLIDING_WINDOW];


	PE::NetworkContext *m_pNetContext;
};
}; // namespace Components
}; // namespace PE
#endif
