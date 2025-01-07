#ifndef __PYENGINE_2_0_EVENTQUEUE_H__
#define __PYENGINE_2_0_EVENTQUEUE_H__

// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

// Outer-Engine includes
#include <assert.h>

// Inter-Engine includes
#include "PrimeEngine/MemoryManagement/Handle.h"
#include "PrimeEngine/PrimitiveTypes/PrimitiveTypes.h"
#include "../Utils/Queue.h"
// Sibling/Children includes

#include "Event.h"
namespace PE {
namespace Events {

enum EventQueueTypes
{
	QT_INVALID = -1,
	QT_TYPELESS = 0,
	QT_GENERAL = 1,
	QT_INPUT = 2
};

// This is an event queue. There will be one global event queue, but it will be allowed
// to create additional queues (i.e. there will not e only one single instance)
struct EventQueue : PE::PEAllocatableAndDefragmentable
{
	// Singleton ------------------------------------------------------------------

	static void Construct()
	{
		Handle handle("EVENT_QUEUE", sizeof(EventQueue));
		/*EventQueue *pEventQueue = */new(handle) EventQueue;
		
		// Singleton
		SetInstanceHandle(handle);
	}

	static EventQueue *Instance()
	{
		return s_myHandle.getObject<EventQueue>();
	}

	static Handle InstanceHandle()
	{
		return s_myHandle;
	}

	static void SetInstanceHandle(const Handle &handle)
	{
		// Singleton
		EventQueue::s_myHandle = handle;
	}

	// Constructor -------------------------------------------------------------
	EventQueue()
	{
	}

	// Methods -----------------------------------------------------------------
	PrimitiveTypes::Bool empty() const {return m_events.isEmpty();}

	void destroy()
	{
		m_events.destroy();
	}

	void add(const Handle hEvt)
	{
		m_events.add(hEvt);
	}

	Handle &getFrontHandle()
	{
		assert(!m_events.isEmpty());
		return *m_events.getFront();
	}

	Event *getFront()
	{
		assert(!m_events.isEmpty());
		return m_events.getFront()->getObject<Event>();
	}

	void destroyFront()
	{
		// make sure that event data is released
		getFrontHandle().release();

		m_events.deleteFront();
	}

private:
	static Handle s_myHandle;
	Queue<Handle> m_events;
public:
};

}; // namespace Events
}; // namepsace PE

#endif
