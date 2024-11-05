#ifndef _TANK_H_
#define _TANK_H_

#include "PrimeEngine/Events/Component.h"
#include "PrimeEngine/Math/Vector3.h"

namespace PE {
    namespace Events{
        struct EventQueueManager;
    }
}

namespace CharacterControl {
namespace Components {

	struct TankGameControls : public PE::Components::Component
	{
		PE_DECLARE_CLASS(TankGameControls);
	public:

		TankGameControls(PE::GameContext &context, PE::MemoryArena arena, PE::Handle hMyself)
			: PE::Components::Component(context, arena, hMyself)
		{
		}

		virtual ~TankGameControls(){}
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
	};

    struct TankController : public PE::Components::Component
    {
        // component API
        PE_DECLARE_CLASS(TankController);
        
        TankController(PE::GameContext &context, PE::MemoryArena arena,
			PE::Handle myHandle, float speed,
			Vector3 spawnPos, float networkPingInterval); // constructor
        
        virtual void addDefaultComponents(); // adds default children and event handlers
        
        
        PE_DECLARE_IMPLEMENT_EVENT_HANDLER_WRAPPER(do_UPDATE);
        virtual void do_UPDATE(PE::Events::Event *pEvt);
        
		PE_DECLARE_IMPLEMENT_EVENT_HANDLER_WRAPPER(do_Tank_Throttle);
		virtual void do_Tank_Throttle(PE::Events::Event *pEvt);

		PE_DECLARE_IMPLEMENT_EVENT_HANDLER_WRAPPER(do_Tank_Turn);
		virtual void do_Tank_Turn(PE::Events::Event *pEvt);


		void overrideTransform(Matrix4x4 &t);
		void activate();

        float m_timeSpeed;
        float m_time;
		float m_networkPingTimer;
		float m_networkPingInterval;
        Vector2 m_center;
        PrimitiveTypes::UInt32 m_counter;
		Vector3 m_spawnPos;
		bool m_active;
		bool m_overriden;
		Matrix4x4 m_transformOverride;

    };
}; // namespace Components
}; // namespace CharacterControl

#endif
