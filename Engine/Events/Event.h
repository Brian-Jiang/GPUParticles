#ifndef __PYENGINE_2_0_EVENT_H__
#define __PYENGINE_2_0_EVENT_H__

// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

// Outer-Engine includes
#include <assert.h>

// Inter-Engine includes
#include "PrimeEngine/MemoryManagement/Handle.h"
#include "PrimeEngine/PrimitiveTypes/PrimitiveTypes.h"

#include "../Utils/PEUUID.h"
#include "../Utils/PEClassDecl.h"


// Sibling/Children includes

struct lua_State;

namespace PE {
namespace Events {

enum EventReturnCodes
{
	DEFAULT = 0,
	STOP_DISTRIBUTION = 1,
};
// Represents event. Has a handle to data specific to an event
struct Event : public PEClass
{
	PE_DECLARE_CLASS(Event);

	// meta methods: wrap around MetaInfo methods
	int getClassId() { return getClassMetaInfo()->m_classId; }
	const char *getClassName() { return getClassMetaInfo()->getClassName(); }

	template <typename T>
	bool isInstanceOf() {
		return getClassMetaInfo()->isSubClassOf(T::s_metaInfo.m_classId);
	}

	bool isInstanceOf(int classId) {
		return getClassMetaInfo()->isSubClassOf(classId);
	}

	void printClassHierarchy()
	{
		return getClassMetaInfo()->printClassHierarchy();
	}

	Event()
	{
		m_returnCode = DEFAULT;
		m_cancelSiblingAndChildEventHandling = false;
		m_networkClientId = -1;
	}
	virtual ~Event(){}
	
	// call this when an event is not used anymore
	void releaseEventData()
	{
		m_dataHandle.release();
	}

	PrimitiveTypes::Bool hasValidData()
	{
		return m_dataHandle.isValid();
	}

	PEUUID m_evtTypePEUUID;
	PrimitiveTypes::UInt32 m_returnCode;
	Handle m_target; // what object this is meant for
	Handle m_lastDistributor; // last event handler in the event handling chain
	Handle m_prevDistributor;
	Handle m_dataHandle;
	bool m_cancelSiblingAndChildEventHandling;
	int m_networkClientId; // in case on server, this id will be which client it came from
};


}; // namespace Events
}; // napespace PE
#endif
