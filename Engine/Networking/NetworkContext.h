
#ifndef __PrimeEngine_NetworkContext_h__
#define __PrimeEngine_NetworkContext_h__

#include "PrimeEngine/MemoryManagement/MemoryPool.h"

struct MainFunctionArgs;

namespace PE {
namespace Components{
	struct ConnectionManager;
	struct EventManager;
	struct StreamManager;
};
struct NetworkContext
{
	NetworkContext()
		: m_pConnectionManager(NULL)
		, m_pEventManager(NULL)
		, m_pStreamManager(NULL)
		, m_clientId(-1)
	{}
	Components::ConnectionManager *getConnectionManager(){return m_pConnectionManager;}
	Components::EventManager *getEventManager(){return m_pEventManager;}
	Components::StreamManager *getStreamManager(){return m_pStreamManager;}
	int getClientId(){return m_clientId;}
	
	Components::ConnectionManager *m_pConnectionManager;
	Components::EventManager *m_pEventManager;
	Components::StreamManager *m_pStreamManager;

	int m_clientId; // id of client in the list of contexts on server. on client is invalid since have only one connection
};

}; // namespace PE

#endif