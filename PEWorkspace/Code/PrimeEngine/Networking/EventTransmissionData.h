#ifndef __PrimeEngineEventTransmissionData_H__
#define __PrimeEngineEventTransmissionData_H__

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


namespace PE {
	namespace Components
	{
		struct Component;
	};
	namespace Events
	{
		struct Event;
	};

struct EventTransmissionData
{
	bool m_isGuaranteed;
	int m_size;
	int m_orderId;
	char m_payload[PE_MAX_EVENT_PAYLOAD];
};

struct EventReceptionData
{
	Components::Component *m_pTargetComponent;
	PE::Events::Event *m_pEvent;
};

}; // namespace PE
#endif
