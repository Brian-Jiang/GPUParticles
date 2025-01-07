#ifndef __PYENGINE_2_0_DX10_XINPUT_H___
#define __PYENGINE_2_0_DX10_XINPUT_H___

// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"
#if APIABSTRACTION_D3D9 || APIABSTRACTION_D3D11

// Outer-Engine includes
#include <XInput.h>
#include <assert.h>

// Inter-Engine includes
#include "PrimeEngine/MemoryManagement/Handle.h"
#include "PrimeEngine/Events/StandardEvents.h"
#include "PrimeEngine/Events/Component.h"
#include "PrimeEngine/Events/EventQueueManager.h"

// Sibling/Children includes

namespace PE {
// File specific #define's
#define XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE  7849
#define XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE 8689
#define XINPUT_GAMEPAD_TRIGGER_THRESHOLD    30

namespace Components {
// This struct polls for controller input and fires off events accordingly
struct DX9_XInput : public Component
{
	PE_DECLARE_CLASS(DX9_XInput);

	DX9_XInput(PE::GameContext &context, PE::MemoryArena arena, Handle hMyself);
	
	// Component ------------------------------------------------------------
	virtual void addDefaultComponents() ;

	// Individual events -------------------------------------------------------
	PE_DECLARE_IMPLEMENT_EVENT_HANDLER_WRAPPER(do_UPDATE);
	virtual void do_UPDATE(Events::Event *pEvt);

	void generateButtonEvents();

	void generateAxisEvents();

	XINPUT_STATE state;
	XINPUT_STATE lastState;
	PE::Events::EventQueueManager *m_pQueueManager;
};
}; // namespace Components
}; // namespace PE

#endif//API abstraction
#endif//File guard
