#ifndef _CLIENT_SPACE_SHIP_H_
#define _CLIENT_SPACE_SHIP_H_

#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

#include "PrimeEngine/Events/Component.h"
#include "PrimeEngine/Math/Vector3.h"
#include "PrimeEngine/Math/Matrix4x4.h"

namespace PE {
    namespace Events{
        struct EventQueueManager;
    }
}

namespace CharacterControl {
namespace Components {

    struct ClientSpaceShip : public PE::Components::Component
    {
        // component API
        PE_DECLARE_CLASS(ClientSpaceShip);
        
        ClientSpaceShip(PE::GameContext &context, PE::MemoryArena arena,
			PE::Handle myHandle, float speed,
			Vector3 spawnPos, float networkPingInterval); // constructor
        
        virtual void addDefaultComponents(); // adds default children and event handlers
        
        
        PE_DECLARE_IMPLEMENT_EVENT_HANDLER_WRAPPER(do_UPDATE);
        virtual void do_UPDATE(PE::Events::Event *pEvt);
       
		void overrideTransform(Matrix4x4 &t);
		void activate();

        float m_timeSpeed;
        float m_time;
		float m_networkPingTimer;
		float m_networkPingInterval;

		Vector3 m_spawnPos;
		bool m_active;
		bool m_overriden;
		Matrix4x4 m_transformOverride;

		float m_rollVel;
		float m_roll;
		float m_cameraRoll;
		float m_throttleVel;
    };
}; // namespace Components
}; // namespace CharacterControl

#endif
