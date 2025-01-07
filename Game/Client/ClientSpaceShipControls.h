#ifndef _CLIENT_SPACE_SHIP_CONTROLS_H_
#define _CLIENT_SPACE_SHIP_CONTROLS_H_

#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"
#include "PrimeEngine/Events/Component.h"

namespace PE {
    namespace Events{
        struct EventQueueManager;
    }
}

namespace CharacterControl {
namespace Components {

	struct SpaceShipGameControls : public PE::Components::Component
	{
		PE_DECLARE_CLASS(SpaceShipGameControls);
	public:

		SpaceShipGameControls(PE::GameContext &context, PE::MemoryArena arena, PE::Handle hMyself)
			: PE::Components::Component(context, arena, hMyself)
		{
		}

		virtual ~SpaceShipGameControls(){}
		// Component ------------------------------------------------------------

		PE_DECLARE_IMPLEMENT_EVENT_HANDLER_WRAPPER(do_UPDATE);
		virtual void do_UPDATE(PE::Events::Event *pEvt);

		virtual void addDefaultComponents() ;

		//Methods----------------
		void handleIOSDebugInputEvents(PE::Events::Event *pEvt);
		void handleKeyboardDebugInputEvents(PE::Events::Event *pEvt);
		void handleControllerDebugInputEvents(PE::Events::Event *pEvt);

		PE::Events::EventQueueManager *m_pQueueManager;

		PrimitiveTypes::Float32 m_frameTime;

		// cache the results of each input event

		float m_throttle;
		float m_roll;
	};

}; // namespace Components
}; // namespace CharacterControl

#endif
