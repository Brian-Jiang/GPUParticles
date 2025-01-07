#ifndef __PYENGINE_DEFAULT_GAME_CONTROLS_H__
#define __PYENGINE_DEFAULT_GAME_CONTROLS_H__

// API Abstraction

// Outer-Engine includes
#include <assert.h>

// Inter-Engine includes
#include "PrimeEngine/MemoryManagement/Handle.h"
#include "../../Events/EventQueueManager.h"
#include "../../Events/Component.h"
#include "../../Events/StandardKeyboardEvents.h"
#include "../../Scene/CameraManager.h"
#include "../../Logging/Log.h"

// definitions of standard game events. the events that any game could potentially use
#include "../../Events/StandardGameEvents.h"

// Sibling/Children includes

namespace PE {
namespace Components {

struct DefaultGameControls : public Component
{
	PE_DECLARE_CLASS(DefaultGameControls);
public:
	
	DefaultGameControls(PE::GameContext &context, PE::MemoryArena arena, Handle hMyself) : Component(context, arena, hMyself)
	{
	}

	virtual ~DefaultGameControls(){}
	// Component ------------------------------------------------------------

	PE_DECLARE_IMPLEMENT_EVENT_HANDLER_WRAPPER(do_UPDATE);
	virtual void do_UPDATE(Events::Event *pEvt);

	virtual void addDefaultComponents() ;

	//Methods----------------
    void handleIOSDebugInputEvents(Events::Event *pEvt);
	void handleKeyboardDebugInputEvents(Events::Event *pEvt);
	void handleControllerDebugInputEvents(Events::Event *pEvt);
	
	Events::EventQueueManager *m_pQueueManager;
	
	PrimitiveTypes::Float32 m_frameTime;
};
}; // namespace Components
}; // namespace PE

#endif//File guard