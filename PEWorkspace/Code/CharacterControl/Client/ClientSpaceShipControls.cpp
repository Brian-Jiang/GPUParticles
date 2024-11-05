#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

#include "ClientSpaceShipControls.h"

// Inter-Engine includes
#include "PrimeEngine/Lua/LuaEnvironment.h"
#include "PrimeEngine/Events/StandardEvents.h"
#include "PrimeEngine/Scene/Mesh.h"
#include "PrimeEngine/Scene/SceneNode.h"
#include "PrimeEngine/Networking/EventManager.h"
#include "PrimeEngine/Networking/Client/ClientNetworkManager.h"
#include "CharacterControl/Events/Events.h"
#include "PrimeEngine/GameObjectModel/GameObjectManager.h"
#include "PrimeEngine/Events/StandardKeyboardEvents.h"
#include "PrimeEngine/Events/StandardIOSEvents.h"
#include "PrimeEngine/Events/StandardGameEvents.h"
#include "PrimeEngine/Events/EventQueueManager.h"
#include "PrimeEngine/Events/StandardControllerEvents.h"
#include "PrimeEngine/GameObjectModel/DefaultGameControls/DefaultGameControls.h"
#include "CharacterControl/CharacterControlContext.h"



using namespace PE::Components;
using namespace PE::Events;
using namespace CharacterControl::Events;

// Arkane Control Values
#define Analog_To_Digital_Trigger_Distance 0.5f
static float Debug_Fly_Speed = 8.0f; //Units per second
#define Debug_Rotate_Speed 2.0f //Radians per second
#define Player_Keyboard_Rotate_Speed 20.0f //Radians per second

namespace CharacterControl {
namespace Components {

	


PE_IMPLEMENT_CLASS1(SpaceShipGameControls, PE::Components::Component);

void SpaceShipGameControls::addDefaultComponents()
{
	Component::addDefaultComponents();

	PE_REGISTER_EVENT_HANDLER(Event_UPDATE, SpaceShipGameControls::do_UPDATE);
}

void SpaceShipGameControls::do_UPDATE(PE::Events::Event *pEvt)
{
	m_roll = 0;
	m_throttle = 0;
	// Process input events (controller	 buttons, triggers...)
	PE::Handle iqh = PE::Events::EventQueueManager::Instance()->getEventQueueHandle("input");
	
	// Process input event -> game event conversion
	while (!iqh.getObject<PE::Events::EventQueue>()->empty())
	{
		PE::Events::Event *pInputEvt = iqh.getObject<PE::Events::EventQueue>()->getFront();
		m_frameTime = ((Event_UPDATE*)(pEvt))->m_frameTime;
		// Have DefaultGameControls translate the input event to GameEvents
		handleKeyboardDebugInputEvents(pInputEvt);
		handleControllerDebugInputEvents(pInputEvt);
        handleIOSDebugInputEvents(pInputEvt);
		
		iqh.getObject<PE::Events::EventQueue>()->destroyFront();
	}

	// Events are destoryed by destroyFront() but this is called every frame just in case
	iqh.getObject<PE::Events::EventQueue>()->destroy();
}
    
void SpaceShipGameControls::handleIOSDebugInputEvents(Event *pEvt)
{
    #if APIABSTRACTION_IOS
    m_pQueueManager = PE::Events::EventQueueManager::Instance();
    if (Event_IOS_TOUCH_MOVED::GetClassId() == pEvt->getClassId())
    {
        Event_IOS_TOUCH_MOVED *pRealEvent = (Event_IOS_TOUCH_MOVED *)(pEvt);
        
        if(pRealEvent->touchesCount > 1)
        {
            PE::Handle h("EVENT", sizeof(Events::Event_Tank_Throttle));
            Events::Event_Tank_Throttle *flyCameraEvt = new(h) Events::Event_Tank_Throttle;
                
            Vector3 relativeMovement(0.0f,0.0f,-30.0f * pRealEvent->m_normalized_dy);
            flyCameraEvt->m_relativeMove = relativeMovement * Debug_Fly_Speed * m_frameTime;
            m_pQueueManager->add(h, QT_GENERAL);
        }
        else
        {
            PE::Handle h("EVENT", sizeof(Event_Tank_Turn));
            Event_Tank_Turn *rotateCameraEvt = new(h) Event_Tank_Turn ;
            
            Vector3 relativeRotate(pRealEvent->m_normalized_dx * 10,0.0f,0.0f);
            rotateCameraEvt->m_relativeRotate = relativeRotate * Debug_Rotate_Speed * m_frameTime;
            m_pQueueManager->add(h, QT_GENERAL);
        }
    }
    #endif
}

void SpaceShipGameControls::handleKeyboardDebugInputEvents(Event *pEvt)
{
	m_pQueueManager = PE::Events::EventQueueManager::Instance();
	/*
	if (PE::Events::Event_KEY_A_HELD::GetClassId() == pEvt->getClassId())
	{
		PE::Handle h("EVENT", sizeof(Event_FLY_CAMERA));
		PE::Events::Event_FLY_CAMERA *flyCameraEvt = new(h) PE::Events::Event_FLY_CAMERA ;
		
		Vector3 relativeMovement(-1.0f,0.0f,0.0f);
		flyCameraEvt->m_relativeMove = relativeMovement * Debug_Fly_Speed * m_frameTime;
		m_pQueueManager->add(h, QT_GENERAL);
	}
	else 
	*/
	if (Event_KEY_S_HELD::GetClassId() == pEvt->getClassId())
	{
		m_throttle = -1.0f * Debug_Fly_Speed * m_frameTime;
	}
	/*
	else if (Event_KEY_D_HELD::GetClassId() == pEvt->getClassId())
	{
		PE::Handle h("EVENT", sizeof(Event_FLY_CAMERA));
		Event_FLY_CAMERA *flyCameraEvt = new(h) Event_FLY_CAMERA ;
		
		Vector3 relativeMovement(1.0f,0.0f,0.0f);
		flyCameraEvt->m_relativeMove = relativeMovement * Debug_Fly_Speed * m_frameTime;
		m_pQueueManager->add(h, QT_GENERAL);
	}*/
	else if (Event_KEY_W_HELD::GetClassId() == pEvt->getClassId())
	{
		m_throttle = 1.0f * Debug_Fly_Speed * m_frameTime;
	}
	else if (Event_KEY_LEFT_HELD::GetClassId() == pEvt->getClassId())
	{
		m_roll = 1.0f;
	}
	else if (Event_KEY_RIGHT_HELD::GetClassId() == pEvt->getClassId())
	{
		m_roll = -1.0f;
	}
	/*
	else if (Event_KEY_DOWN_HELD::GetClassId() == pEvt->getClassId())
	{
		PE::Handle h("EVENT", sizeof(Event_ROTATE_CAMERA));
		Event_ROTATE_CAMERA *rotateCameraEvt = new(h) Event_ROTATE_CAMERA ;
		
		Vector3 relativeRotate(0.0f,-1.0f,0.0f);
		rotateCameraEvt->m_relativeRotate = relativeRotate * Debug_Rotate_Speed * m_frameTime;
		m_pQueueManager->add(h, QT_GENERAL);
	}
	else if (Event_KEY_UP_HELD::GetClassId() == pEvt->getClassId())
	{
		PE::Handle h("EVENT", sizeof(Event_ROTATE_CAMERA));
		Event_ROTATE_CAMERA *rotateCameraEvt = new(h) Event_ROTATE_CAMERA ;
		
		Vector3 relativeRotate(0.0f,1.0f,0.0f);
		rotateCameraEvt->m_relativeRotate = relativeRotate * Debug_Rotate_Speed * m_frameTime;
		m_pQueueManager->add(h, QT_GENERAL);
	}
	*/
	else
	{
		Component::handleEvent(pEvt);
	}
}

void SpaceShipGameControls::handleControllerDebugInputEvents(Event *pEvt)
{
	if (Event_ANALOG_L_THUMB_MOVE::GetClassId() == pEvt->getClassId())
	{
		Event_ANALOG_L_THUMB_MOVE *pRealEvent = (Event_ANALOG_L_THUMB_MOVE*)(pEvt);
		
		//throttle
		{
			PE::Handle h("EVENT", sizeof(Events::Event_Tank_Throttle));
			Events::Event_Tank_Throttle *flyCameraEvt = new(h) Events::Event_Tank_Throttle ;

			Vector3 relativeMovement(0.0f,0.0f, pRealEvent->m_absPosition.getY());
			flyCameraEvt->m_relativeMove = relativeMovement * Debug_Fly_Speed * m_frameTime;
			m_pQueueManager->add(h, QT_GENERAL);
		}

		//turn
		{
			PE::Handle h("EVENT", sizeof(Event_Tank_Turn));
			Event_Tank_Turn *rotateCameraEvt = new(h) Event_Tank_Turn ;

			Vector3 relativeRotate(pRealEvent->m_absPosition.getX(), 0.0f, 0.0f);
			rotateCameraEvt->m_relativeRotate = relativeRotate * Debug_Rotate_Speed * m_frameTime;
			m_pQueueManager->add(h, QT_GENERAL);
		}
	}
	/*
	else if (Event_ANALOG_R_THUMB_MOVE::GetClassId() == pEvt->getClassId())
	{
		Event_ANALOG_R_THUMB_MOVE *pRealEvent = (Event_ANALOG_R_THUMB_MOVE *)(pEvt);
		
		PE::Handle h("EVENT", sizeof(Event_ROTATE_CAMERA));
		Event_ROTATE_CAMERA *rotateCameraEvt = new(h) Event_ROTATE_CAMERA ;
		
		Vector3 relativeRotate(pRealEvent->m_absPosition.getX(), pRealEvent->m_absPosition.getY(), 0.0f);
		rotateCameraEvt->m_relativeRotate = relativeRotate * Debug_Rotate_Speed * m_frameTime;
		m_pQueueManager->add(h, QT_GENERAL);
	}
	else if (Event_PAD_N_DOWN::GetClassId() == pEvt->getClassId())
	{
	}
	else if (Event_PAD_N_HELD::GetClassId() == pEvt->getClassId())
	{
		
	}
	else if (Event_PAD_N_UP::GetClassId() == pEvt->getClassId())
	{
		
	}
	else if (Event_PAD_S_DOWN::GetClassId() == pEvt->getClassId())
	{
		
	}
	else if (Event_PAD_S_HELD::GetClassId() == pEvt->getClassId())
	{
		
	}
	else if (Event_PAD_S_UP::GetClassId() == pEvt->getClassId())
	{
		
	}
	else if (Event_PAD_W_DOWN::GetClassId() == pEvt->getClassId())
	{
		
	}
	else if (Event_PAD_W_HELD::GetClassId() == pEvt->getClassId())
	{
		
	}
	else if (Event_PAD_W_UP::GetClassId() == pEvt->getClassId())
	{
		
	}
	else if (Event_PAD_E_DOWN::GetClassId() == pEvt->getClassId())
	{
		
	}
	else if (Event_PAD_E_HELD::GetClassId() == pEvt->getClassId())
	{
		
	}
	else if (Event_PAD_E_UP::GetClassId() == pEvt->getClassId())
	{
		
	}
	else if (Event_BUTTON_A_HELD::GetClassId() == pEvt->getClassId())
	{
		
	}
	else if (Event_BUTTON_Y_DOWN::GetClassId() == pEvt->getClassId())
	{
		
	}
	else if (Event_BUTTON_A_UP::GetClassId() == pEvt->getClassId())
	{
		
	}
	else if (Event_BUTTON_B_UP::GetClassId() == pEvt->getClassId())
	{
		
	}
	else if (Event_BUTTON_X_UP::GetClassId() == pEvt->getClassId())
	{
		
	}
	else if (Event_BUTTON_Y_UP::GetClassId() == pEvt->getClassId())
	{
		
	}
	else if (Event_ANALOG_L_TRIGGER_MOVE::GetClassId() == pEvt->getClassId())
	{
		
	}
	else if (Event_ANALOG_R_TRIGGER_MOVE::GetClassId() == pEvt->getClassId())
	{
		
	}
	else if (Event_BUTTON_L_SHOULDER_DOWN::GetClassId() == pEvt->getClassId())
	{
		
	}
	*/
	else
	
	{
		Component::handleEvent(pEvt);
	}
}

}
}
