#define NOMINMAX
// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

#include "EventManager.h"

// Outer-Engine includes

// Inter-Engine includes

#include "../Lua/LuaEnvironment.h"

// additional lua includes needed
extern "C"
{
#include "../../luasocket_dist/src/socket.h"
#include "../../luasocket_dist/src/inet.h"
};

#include "../../../GlobalConfig/GlobalConfig.h"

#include "PrimeEngine/Events/StandardEvents.h"
#include "PrimeEngine/Networking/NetworkManager.h"

#include "PrimeEngine/Scene/DebugRenderer.h"

#include "StreamManager.h"
// Sibling/Children includes
using namespace PE::Events;

namespace PE {
namespace Components {

PE_IMPLEMENT_CLASS1(EventManager, Component);

EventManager::EventManager(PE::GameContext &context, PE::MemoryArena arena, PE::NetworkContext &netContext, Handle hMyself)
: Component(context, arena, hMyself)
, m_transmitterNextEvtOrderId(1) // start at 1 since id = 0 is not ordered
, m_transmitterNumEventsNotAcked(0)

// receiver
, m_receiverFirstEvtOrderId(1) // start at 1 since id = 0 is not ordered
{
	m_pNetContext = &netContext;

	memset(&m_receivedEvents[0], 0, sizeof(m_receivedEvents));
}

EventManager::~EventManager()
{

}

void EventManager::initialize()
{

}

void EventManager::addDefaultComponents()
{
	Component::addDefaultComponents();
}

void EventManager::scheduleEvent(PE::Networkable *pNetworkableEvent, PE::Networkable *pNetworkableTarget, bool guaranteed)
{
	if (haveEventsToSend() >= PE_MAX_EVENT_JAM)
	{
		assert(!"Sending too many events have to drop, need throttling mechanism here");
		return;
	}

	m_eventsToSend.push_back(EventTransmissionData());
	EventTransmissionData &back = m_eventsToSend.back();
	int dataSize = 0;

	back.m_isGuaranteed = guaranteed;
	if (!guaranteed)
		back.m_orderId = 0; // zero means not guaranteed
	else
		back.m_orderId = m_transmitterNextEvtOrderId++;

	// debug info to show event id sceduled
	//PEINFO("Scheduling event order id: %d\n", m_transmitterNextEvtOrderId);


	//write ordering id (0 = not guaranteed)
	dataSize += StreamManager::WriteInt32(back.m_orderId, &back.m_payload[dataSize]);
	
	//target
	dataSize += StreamManager::WriteNetworkId(pNetworkableTarget->m_networkId, &back.m_payload[dataSize]);

	PrimitiveTypes::Int32 classId = pNetworkableEvent->net_getClassMetaInfo()->m_classId;

	if (classId == -1)
	{
		assert(!"Event's class id is -1, need to add it to global registry");
	}

	dataSize += StreamManager::WriteInt32(classId, &back.m_payload[dataSize]);
	
	dataSize += pNetworkableEvent->packCreationData(&back.m_payload[dataSize]);
	
	back.m_size = dataSize;
	
}

int EventManager::haveEventsToSend()
{
	return m_eventsToSend.size() > 0;
}

int EventManager::fillInNextPacket(char *pDataStream, TransmissionRecord *pRecord, int packetSizeAllocated, bool &out_usefulDataSent, bool &out_wantToSendMore)
{
	out_usefulDataSent = false;
    out_wantToSendMore = false;

	int eventsToSend = haveEventsToSend();
	assert(eventsToSend);

	int eventsReallySent = 0;

	int size = 0;
	size += StreamManager::WriteInt32(eventsToSend, &pDataStream[size]);

	int sizeLeft = packetSizeAllocated - size;

	for (int i = 0; i < eventsToSend; ++i)
	{
		int iEvt = i;
		assert(iEvt < (int)(m_eventsToSend.size()));
		EventTransmissionData &evt = m_eventsToSend[iEvt];

		if (evt.m_size > sizeLeft)
		{
			// can't fit this event, break out
			// note this code can be optimized to include next events that can potentailly fit in
            out_wantToSendMore = true;
			break;
		}

		// store this to be able to resolve which events were delivered or dropped on transmittion notification
		// todo: optimize to use pointers and store data somewhere else
		pRecord->m_sentEvents.push_back(evt);

		//int checkOrderId = 0;
		//StreamManager::ReadInt32( &evt.m_payload[0], checkOrderId);
		//debug info to show event order id of packet being sent
		//PEINFO("Order id check: %d\n", checkOrderId);
		
		memcpy(&pDataStream[size], &evt.m_payload[0], evt.m_size);
		size += evt.m_size;
		sizeLeft = packetSizeAllocated - size;

		eventsReallySent++;
		m_transmitterNumEventsNotAcked++;
	}
	
	if (eventsReallySent > 0)
	{
		m_eventsToSend.erase(m_eventsToSend.begin(), m_eventsToSend.begin() + eventsReallySent);
	}
	
	//write real value into the beginning of event chunk
	StreamManager::WriteInt32(eventsReallySent, &pDataStream[0 /* the number of events is stored in the beginning and we already wrote value into here*/]);
	
	// we are sending useful data only if we are sending events
	out_usefulDataSent = eventsReallySent > 0;

	return size;
}

void EventManager::processNotification(TransmissionRecord *pTransmittionRecord, bool delivered)
{
	for (unsigned int i = 0; i < pTransmittionRecord->m_sentEvents.size(); ++i)
	{
		EventTransmissionData &evt = pTransmittionRecord->m_sentEvents[i];

		if (evt.m_isGuaranteed)
		{
			if (delivered)
			{
				//we're good, can pop this event off front
				m_transmitterNumEventsNotAcked--; // will advance sliding window
			}
			else
			{
				// need to readjust our sliding window and make sure we start sending events starting at at least this event
				assert(!"Not supported for now!");
				m_eventsToSend.push_front(evt);
				m_transmitterNumEventsNotAcked--; // will advance sliding window since we need to resend this event
			}
		}
		else
		{
			m_transmitterNumEventsNotAcked--; // will advance sliding window

			// event wasn't guaranteed, we can forget about it
		}
	}
}

void EventManager::debugRender(int &threadOwnershipMask, float xoffset/* = 0*/, float yoffset/* = 0*/)
{
	float dy = 0.025f;
	float dx = 0.01f;
	sprintf(PEString::s_buf, "Event Manager:");
	DebugRenderer::Instance()->createTextMesh(
		PEString::s_buf, true, false, false, false, 0,
		Vector3(xoffset, yoffset, 0), 1.0f, threadOwnershipMask);

	sprintf(PEString::s_buf, "Recv Window Range: [%d, %d]", m_receiverFirstEvtOrderId, m_receiverFirstEvtOrderId + PE_EVENT_SLIDING_WINDOW -1);
	DebugRenderer::Instance()->createTextMesh(
		PEString::s_buf, true, false, false, false, 0,
		Vector3(xoffset + dx, yoffset + dy, 0), 1.0f, threadOwnershipMask);

	char tmpBuf[1024];
	char tmpBuf2[1024];
	
	sprintf(tmpBuf, "%s", "[");
	
	for (int i = 0; i < PE_EVENT_SLIDING_WINDOW; ++i)
	{
		if (m_receivedEvents[i].m_pEvent)
		{
			sprintf(tmpBuf2, "%s+", tmpBuf);
		}
		else
		{
			sprintf(tmpBuf2, "%s ", tmpBuf);
		}
		sprintf(tmpBuf, "%s", tmpBuf2);
	}

	sprintf(tmpBuf2, "%s]", tmpBuf);
	sprintf(tmpBuf, "%s", tmpBuf2);

	DebugRenderer::Instance()->createTextMesh(
		tmpBuf, true, false, false, false, 0,
		Vector3(xoffset + dx, yoffset + dy * 2, 0), 0.7f, threadOwnershipMask);

	sprintf(PEString::s_buf, "Send next id: %d", m_transmitterNextEvtOrderId);
	DebugRenderer::Instance()->createTextMesh(
		PEString::s_buf, true, false, false, false, 0,
		Vector3(xoffset + dx, yoffset + dy * 3, 0), 1.0f, threadOwnershipMask);

}


int EventManager::receiveNextPacket(char *pDataStream)
{
	int read = 0;
	PrimitiveTypes::Int32 numEvents;
	
	read += StreamManager::ReadInt32(&pDataStream[read], numEvents);

	for (int i = 0; i < numEvents; ++i)
	{
		PrimitiveTypes::Int32 evtOrderId;
		read += StreamManager::ReadInt32(&pDataStream[read], evtOrderId); // 0 means not guaranteed, > 0 means ordering id
		
		Networkable::NetworkId networkId;
		read += StreamManager::ReadNetworkId(&pDataStream[read], networkId);
		Networkable *pTargetNetworkable = NULL;
		Component *pTargetComponent = NULL;

		// todo: retrieve object to send event to
		if ((pTargetNetworkable = m_pContext->getNetworkManager()->getNetworkableObject(networkId)))
		{
			MetaInfo *classMetaInfo = pTargetNetworkable->net_getClassMetaInfo();
			if (classMetaInfo->isSubClassOf(Component::GetClassId()))
			{
				//can safely case to component
				pTargetComponent = (Component*)(pTargetNetworkable->getPointerToMainClass());
			}
			else
			{
				assert(!"Unknown Class. Is it inherited from not a Component? Event handlers have to be components");
			}
		}
		else
		{
			assert(!"Network id was not registered with any object! Event will be dismissed");
		}

		PrimitiveTypes::Int32 classId;
		read += StreamManager::ReadInt32(&pDataStream[read], classId);

		GlobalRegistry *globalRegistry = GlobalRegistry::Instance();
		MetaInfo *pMetaInfo = globalRegistry->getMetaInfo(classId);
		if (!pMetaInfo->getFactoryConstructFunction())
		{
			assert(!"Received network creation command but don't have factory create function associated with the given class");
		}

		void *p = (pMetaInfo->getFactoryConstructFunction())(*m_pContext, m_arena);
		if (!p)
			assert(!"Factory construct function returned null");
		
		Events::Event *pEvt = (Events::Event *)(p);
	
		read += pEvt->constructFromStream(&pDataStream[read]);
		pEvt->m_networkClientId = m_pNetContext->getClientId(); // will be id of client on server, or -1 on client

		
		// debug to show id of received event
		//PEINFO("Received Event with eventOrderId %d\n", evtOrderId);

		if (evtOrderId > 0)
		{
			// this is an ordered guaranteed event
			
			// is it within sliding window?
			int indexInEventsArray = evtOrderId - m_receiverFirstEvtOrderId;
			if (indexInEventsArray < 0)
			{
				// received an old event, discard
				PEASSERT(false, "Received event order id %d that precedes current sliding window currently starting at %d. We dont support this situation (old events)\n", evtOrderId, m_receiverFirstEvtOrderId);
				delete pEvt;
			}
			else if (indexInEventsArray >= PE_EVENT_SLIDING_WINDOW)
			{
				// received event too much in advance, have to discard
				PEASSERT(false, "Received event too far in advance of current sliding window. We dont have mechanism to handle this yet, so make sliding window bigger. \
								Current sliding window starts at %d, order of this event is %d. Sliding window size: \n", m_receiverFirstEvtOrderId, evtOrderId, PE_EVENT_SLIDING_WINDOW);
				delete pEvt;
			}
			else if (m_receivedEvents[indexInEventsArray].m_pEvent)
			{
				// this event has already been received. discard
				PEASSERT(false, "Received event order id %d that has already been received We dont support this situation\n", evtOrderId);
				delete pEvt;
			}
			else
			{
				m_receivedEvents[indexInEventsArray].m_pEvent = pEvt;
				m_receivedEvents[indexInEventsArray].m_pTargetComponent = pTargetComponent;
			}

		}
		else
		{
			// this is not guaranteed event (execute and forget)
			pTargetComponent->handleEvent(pEvt);

			delete pEvt;
		}
	}

	// check receiver sliding window and process events if have events for needed order ids

	int numProcessed = 0;
	for (int indexInEventsArray = 0; indexInEventsArray < PE_EVENT_SLIDING_WINDOW; ++indexInEventsArray)
	{
		if (m_receivedEvents[indexInEventsArray].m_pEvent)
		{
			m_receivedEvents[indexInEventsArray].m_pTargetComponent->handleEvent(m_receivedEvents[indexInEventsArray].m_pEvent);
			numProcessed++;
			delete m_receivedEvents[indexInEventsArray].m_pEvent;
		}
		else
			break;
	}

	//shift leftover events if can
	if (numProcessed && numProcessed < PE_EVENT_SLIDING_WINDOW)
	{
		int numLeftoverEvents = PE_EVENT_SLIDING_WINDOW-numProcessed;
		memmove(&m_receivedEvents[0], &m_receivedEvents[numProcessed], sizeof(EventReceptionData) * numLeftoverEvents);
		memset(&m_receivedEvents[numLeftoverEvents], 0, sizeof(EventReceptionData) * (PE_EVENT_SLIDING_WINDOW-numLeftoverEvents));
	}

	m_receiverFirstEvtOrderId += numProcessed; // advance sliding window
	
	return read;
}

#if 0 // template
//////////////////////////////////////////////////////////////////////////
// ConnectionManager Lua Interface
//////////////////////////////////////////////////////////////////////////
//
void ConnectionManager::SetLuaFunctions(PE::Components::LuaEnvironment *pLuaEnv, lua_State *luaVM)
{
	/*
	static const struct luaL_Reg l_functions[] = {
		{"l_clientConnectToTCPServer", l_clientConnectToTCPServer},
		{NULL, NULL} // sentinel
	};

	luaL_register(luaVM, 0, l_functions);
	*/

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
#endif
//////////////////////////////////////////////////////////////////////////

	
}; // namespace Components
}; // namespace PE
