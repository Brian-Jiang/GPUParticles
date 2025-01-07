#ifndef __PYENGINE_2_0_STANDARD_CONROLLER_EVENTS_H__
#define __PYENGINE_2_0_STANDARD_CONROLLER_EVENTS_H__

// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

// Outer-Engine includes

// Inter-Engine includes
#include "../Math/Vector3.h"

// Sibling/Children includes
#include "Event.h"

namespace PE {
namespace Events {

// Button down events
struct Event_BUTTON_Y_DOWN : public Event {
	PE_DECLARE_CLASS(Event_BUTTON_Y_DOWN);
	virtual ~Event_BUTTON_Y_DOWN() {}
};

struct Event_BUTTON_B_DOWN : public Event {
	PE_DECLARE_CLASS(Event_BUTTON_B_DOWN);
	virtual ~Event_BUTTON_B_DOWN() {}
};

struct Event_BUTTON_A_DOWN : public Event {
	PE_DECLARE_CLASS(Event_BUTTON_A_DOWN);
	virtual ~Event_BUTTON_A_DOWN() {}
};

struct Event_BUTTON_X_DOWN : public Event {
	PE_DECLARE_CLASS(Event_BUTTON_X_DOWN);
	virtual ~Event_BUTTON_X_DOWN() {}
};

struct Event_BUTTON_L_THUMB_DOWN : public Event {
	PE_DECLARE_CLASS(Event_BUTTON_L_THUMB_DOWN);
	virtual ~Event_BUTTON_L_THUMB_DOWN() {}
};

struct Event_BUTTON_R_THUMB_DOWN : public Event {
	PE_DECLARE_CLASS(Event_BUTTON_R_THUMB_DOWN);
	virtual ~Event_BUTTON_R_THUMB_DOWN() {}
};

struct Event_BUTTON_L_SHOULDER_DOWN : public Event {
	PE_DECLARE_CLASS(Event_BUTTON_L_SHOULDER_DOWN);
	virtual ~Event_BUTTON_L_SHOULDER_DOWN() {}
};

struct Event_BUTTON_R_SHOULDER_DOWN : public Event {
	PE_DECLARE_CLASS(Event_BUTTON_R_SHOULDER_DOWN);
	virtual ~Event_BUTTON_R_SHOULDER_DOWN() {}
};

struct Event_BUTTON_BACK_DOWN : public Event {
	PE_DECLARE_CLASS(Event_BUTTON_BACK_DOWN);
	virtual ~Event_BUTTON_BACK_DOWN() {}
};

struct Event_BUTTON_START_DOWN : public Event {
	PE_DECLARE_CLASS(Event_BUTTON_START_DOWN);
	virtual ~Event_BUTTON_START_DOWN() {}
};

struct Event_PAD_N_DOWN : public Event {
	PE_DECLARE_CLASS(Event_PAD_N_DOWN);
	virtual ~Event_PAD_N_DOWN() {}
};

struct Event_PAD_S_DOWN : public Event {
	PE_DECLARE_CLASS(Event_PAD_S_DOWN);
	virtual ~Event_PAD_S_DOWN() {}
};

struct Event_PAD_E_DOWN : public Event {
	PE_DECLARE_CLASS(Event_PAD_E_DOWN);
	virtual ~Event_PAD_E_DOWN() {}
};

struct Event_PAD_W_DOWN : public Event {
	PE_DECLARE_CLASS(Event_PAD_W_DOWN);
	virtual ~Event_PAD_W_DOWN() {}
};

// Button up events
struct Event_BUTTON_Y_UP : public Event {
	PE_DECLARE_CLASS(Event_BUTTON_Y_UP);
	virtual ~Event_BUTTON_Y_UP() {}
};

struct Event_BUTTON_B_UP : public Event {
	PE_DECLARE_CLASS(Event_BUTTON_B_UP);
	virtual ~Event_BUTTON_B_UP() {}
};

struct Event_BUTTON_A_UP : public Event {
	PE_DECLARE_CLASS(Event_BUTTON_A_UP);
	virtual ~Event_BUTTON_A_UP() {}
};

struct Event_BUTTON_X_UP : public Event {
	PE_DECLARE_CLASS(Event_BUTTON_X_UP);
	virtual ~Event_BUTTON_X_UP() {}
};

struct Event_BUTTON_L_THUMB_UP : public Event {
	PE_DECLARE_CLASS(Event_BUTTON_L_THUMB_UP);
	virtual ~Event_BUTTON_L_THUMB_UP() {}
};

struct Event_BUTTON_R_THUMB_UP : public Event {
	PE_DECLARE_CLASS(Event_BUTTON_R_THUMB_UP);
	virtual ~Event_BUTTON_R_THUMB_UP() {}
};

struct Event_BUTTON_L_SHOULDER_UP : public Event {
	PE_DECLARE_CLASS(Event_BUTTON_L_SHOULDER_UP);
	virtual ~Event_BUTTON_L_SHOULDER_UP() {}
};

struct Event_BUTTON_R_SHOULDER_UP : public Event {
	PE_DECLARE_CLASS(Event_BUTTON_R_SHOULDER_UP);
	virtual ~Event_BUTTON_R_SHOULDER_UP() {}
};

struct Event_BUTTON_BACK_UP : public Event {
	PE_DECLARE_CLASS(Event_BUTTON_BACK_UP);
	virtual ~Event_BUTTON_BACK_UP() {}
};

struct Event_BUTTON_START_UP : public Event {
	PE_DECLARE_CLASS(Event_BUTTON_START_UP);
	virtual ~Event_BUTTON_START_UP() {}
};

struct Event_PAD_N_UP : public Event {
	PE_DECLARE_CLASS(Event_PAD_N_UP);
	virtual ~Event_PAD_N_UP() {}
};

struct Event_PAD_S_UP : public Event {
	PE_DECLARE_CLASS(Event_PAD_S_UP);
	virtual ~Event_PAD_S_UP() {}
};

struct Event_PAD_E_UP : public Event {
	PE_DECLARE_CLASS(Event_PAD_E_UP);
	virtual ~Event_PAD_E_UP() {}
};

struct Event_PAD_W_UP : public Event {
	PE_DECLARE_CLASS(Event_PAD_W_UP);
	virtual ~Event_PAD_W_UP() {}
};

// Thumb & trigger move events
struct Event_ANALOG_L_THUMB_MOVE : public Event {
	PE_DECLARE_CLASS(Event_ANALOG_L_THUMB_MOVE);
	
	Event_ANALOG_L_THUMB_MOVE(){}
	virtual ~Event_ANALOG_L_THUMB_MOVE() {}

	Vector3 m_absPosition;
};
struct Event_ANALOG_R_THUMB_MOVE : public Event {
	PE_DECLARE_CLASS(Event_ANALOG_R_THUMB_MOVE);

	Event_ANALOG_R_THUMB_MOVE(){}
	virtual ~Event_ANALOG_R_THUMB_MOVE() {}

	Vector3 m_absPosition;
};
struct Event_ANALOG_L_TRIGGER_MOVE : public Event {
	PE_DECLARE_CLASS(Event_ANALOG_L_TRIGGER_MOVE);

	Event_ANALOG_L_TRIGGER_MOVE():m_absPosition(0){}
	virtual ~Event_ANALOG_L_TRIGGER_MOVE() {}

	PrimitiveTypes::Float32 m_absPosition;
};
struct Event_ANALOG_R_TRIGGER_MOVE : public Event {
	PE_DECLARE_CLASS(Event_ANALOG_R_TRIGGER_MOVE);

	Event_ANALOG_R_TRIGGER_MOVE():m_absPosition(0){}

	virtual ~Event_ANALOG_R_TRIGGER_MOVE() {}

	PrimitiveTypes::Float32 m_absPosition;
};

	// Button held events
struct Event_BUTTON_Y_HELD : public Event {
	PE_DECLARE_CLASS(Event_BUTTON_Y_HELD);

	virtual ~Event_BUTTON_Y_HELD() {}
};

struct Event_BUTTON_B_HELD : public Event {
	PE_DECLARE_CLASS(Event_BUTTON_B_HELD);
	virtual ~Event_BUTTON_B_HELD() {}
};

struct Event_BUTTON_A_HELD : public Event {
	PE_DECLARE_CLASS(Event_BUTTON_A_HELD);
	virtual ~Event_BUTTON_A_HELD() {}
};

struct Event_BUTTON_X_HELD : public Event {
	PE_DECLARE_CLASS(Event_BUTTON_X_HELD);
	virtual ~Event_BUTTON_X_HELD() {}
};

struct Event_BUTTON_L_THUMB_HELD : public Event {
	PE_DECLARE_CLASS(Event_BUTTON_L_THUMB_HELD);
	virtual ~Event_BUTTON_L_THUMB_HELD() {}
};

struct Event_BUTTON_R_THUMB_HELD : public Event {
	PE_DECLARE_CLASS(Event_BUTTON_R_THUMB_HELD);
	virtual ~Event_BUTTON_R_THUMB_HELD() {}
};

struct Event_BUTTON_L_SHOULDER_HELD : public Event {
	PE_DECLARE_CLASS(Event_BUTTON_L_SHOULDER_HELD);
	virtual ~Event_BUTTON_L_SHOULDER_HELD() {}
};

struct Event_BUTTON_R_SHOULDER_HELD : public Event {
	PE_DECLARE_CLASS(Event_BUTTON_R_SHOULDER_HELD);
	virtual ~Event_BUTTON_R_SHOULDER_HELD() {}
};

struct Event_BUTTON_BACK_HELD : public Event {
	PE_DECLARE_CLASS(Event_BUTTON_BACK_HELD);
	virtual ~Event_BUTTON_BACK_HELD() {}
};

struct Event_BUTTON_START_HELD : public Event {
	PE_DECLARE_CLASS(Event_BUTTON_START_HELD);
	virtual ~Event_BUTTON_START_HELD() {}
};

struct Event_PAD_N_HELD : public Event {
	PE_DECLARE_CLASS(Event_PAD_N_HELD);
	virtual ~Event_PAD_N_HELD() {}
};

struct Event_PAD_S_HELD : public Event {
	PE_DECLARE_CLASS(Event_PAD_S_HELD);
	virtual ~Event_PAD_S_HELD() {}
};

struct Event_PAD_E_HELD : public Event {
	PE_DECLARE_CLASS(Event_PAD_E_HELD);
	virtual ~Event_PAD_E_HELD() {}
};

struct Event_PAD_W_HELD : public Event {
	PE_DECLARE_CLASS(Event_PAD_W_HELD);
	virtual ~Event_PAD_W_HELD() {}
};

};
}; // namespace Events

#endif
