#ifndef __PYENGINE_STANDARD_GAME_EVENTS_H__
#define __PYENGINE_STANDARD_GAME_EVENTS_H__

// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

// Outer-Engine includes

// Inter-Engine includes
#include "StandardEvents.h"


// Sibling/Children includes

namespace PE {

namespace Events {

struct Event_FLY_CAMERA : public Event {
	PE_DECLARE_CLASS(Event_FLY_CAMERA);

	Event_FLY_CAMERA(){}
	virtual ~Event_FLY_CAMERA(){}

	Vector3 m_relativeMove;
};

struct Event_ROTATE_CAMERA : public Event {
	PE_DECLARE_CLASS(Event_ROTATE_CAMERA);

	Event_ROTATE_CAMERA() {}
	virtual ~Event_ROTATE_CAMERA(){}

	Vector3 m_relativeRotate; //2D screenspace rotate
};

}; // namespace Events
}; // namespace PE

#endif
