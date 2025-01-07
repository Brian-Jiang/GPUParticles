#ifndef __PYENGINE_2_0_EVENTQUEUEMANAGER_H__
#define __PYENGINE_2_0_EVENTQUEUEMANAGER_H__

// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

// Outer-Engine includes
#include <assert.h>

// Inter-Engine includes
#include "PrimeEngine/MemoryManagement/Handle.h"
#include "PrimeEngine/PrimitiveTypes/PrimitiveTypes.h"
#include "../Utils/StrToHandleMap.h"

// Sibling/Children includes
#include "EventQueue.h"

namespace PE {
namespace Events {
// This is an event queue. There will be one global event queue, but it will be allowed
// to create additional queues (i.e. there will not e only one single instance)
struct EventQueueManager : PE::PEAllocatableAndDefragmentable
{
	// Singleton ------------------------------------------------------------------

	static void Construct(PE::GameContext &context, PE::MemoryArena arena)
	{
        PE::Handle handle("EVENT_QUEUE_MANAGER", sizeof(EventQueueManager));
		/*EventQueueManager *pEventQueueManager = */ new(handle) EventQueueManager(context, arena);
		
		// Singleton
		SetInstanceHandle(handle);
	}

	static EventQueueManager *Instance()
	{
		return s_myHandle.getObject<EventQueueManager>();
	}

	static Handle InstanceHandle()
	{
		return s_myHandle;
	}

	static void SetInstanceHandle(const Handle &handle)
	{
		// Singleton
		EventQueueManager::s_myHandle = handle;
	}

	// Constructor -------------------------------------------------------------
	EventQueueManager(PE::GameContext &context, PE::MemoryArena arena);

	// Methods -----------------------------------------------------------------

	void add(PE::Handle hEvt, PrimitiveTypes::UInt32 queueType = Events::QT_GENERAL);

    PE::Handle getEventQueueHandle(const char *pEvtQueueName)
	{
		return m_map.findHandle(pEvtQueueName);
	}

private:
	static PE::Handle s_myHandle;
	StrToHandleMap m_map; 

	Events::EventQueue *generalEvtQueue;
	Events::EventQueue *inputEvtQueue;

public:
};

}; // namespace Events
}; // namespace PE
#endif
