#ifndef __PYENGINE_2_0_DX9_KeyboardMouse_H___
#define __PYENGINE_2_0_DX9_KeyboardMouse_H___

// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"
#if APIABSTRACTION_D3D9 || APIABSTRACTION_D3D11 || (APIABSTRACTION_OGL)

// Outer-Engine includes
#include <assert.h>

// Inter-Engine includes
#include "PrimeEngine/MemoryManagement/Handle.h"
#include "PrimeEngine/Events/EventQueueManager.h"
#include "PrimeEngine/Events/Component.h"

// Sibling/Children includes
// File specific #define's

namespace PE {
namespace Components {

// This struct polls for controller input and fires off events accordingly
struct DX9_KeyboardMouse : public Component
{
	PE_DECLARE_CLASS(DX9_KeyboardMouse);
public:
	
	DX9_KeyboardMouse(PE::GameContext &context, PE::MemoryArena arena, Handle hMyself): Component(context, arena, hMyself)
	{}
	// Component ------------------------------------------------------------

	virtual void addDefaultComponents() ;

	// Individual events -------------------------------------------------------
	PE_DECLARE_IMPLEMENT_EVENT_HANDLER_WRAPPER(do_UPDATE);
	virtual void do_UPDATE(Events::Event *pEvt);

	void generateButtonEvents();

	Events::EventQueueManager *m_pQueueManager;
};

}; // namespace Components
}; // namespace PE

#endif//API abstraction
#endif//File guard