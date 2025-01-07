#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

#include "ClientSpaceShip.h"

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
#include "CharacterControl/Tank/ClientTank.h"
#include "CharacterControl/Client/ClientSpaceShipControls.h"


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

PE_IMPLEMENT_CLASS1(ClientSpaceShip, Component);
    
ClientSpaceShip::ClientSpaceShip(PE::GameContext &context, PE::MemoryArena arena,
	PE::Handle myHandle, float speed, Vector3 spawnPos,
	float networkPingInterval)
: Component(context, arena, myHandle)
, m_timeSpeed(speed)
, m_time(0)
, m_active(0)
, m_networkPingTimer(0)
, m_networkPingInterval(networkPingInterval)
, m_overriden(false)
{
	m_spawnPos = spawnPos;
	m_roll = 0;
	m_rollVel = 0;
	m_cameraRoll = 0;
}
    
void ClientSpaceShip::addDefaultComponents()
{
    Component::addDefaultComponents();
        
    PE_REGISTER_EVENT_HANDLER(PE::Events::Event_UPDATE, ClientSpaceShip::do_UPDATE);

	// note: these event handlers will be registered only when one tank is activated as client tank (i.e. driven by client input on this machine)
// 	PE_REGISTER_EVENT_HANDLER(Event_Tank_Throttle, TankController::do_Tank_Throttle);
// 	PE_REGISTER_EVENT_HANDLER(Event_Tank_Turn, TankController::do_Tank_Turn);

}

void ClientSpaceShip::do_UPDATE(PE::Events::Event *pEvt)
{
	PE::Events::Event_UPDATE *pRealEvt = (PE::Events::Event_UPDATE *)(pEvt);

	PE::Handle hFisrtSN = getFirstComponentHandle<SceneNode>();
	if (!hFisrtSN.isValid())
	{
		assert(!"wrong setup. must have scene node referenced");
		return;
	}

	SceneNode *pFirstSN = hFisrtSN.getObject<SceneNode>();

	if (m_active)
	{
		m_time += pRealEvt->m_frameTime;
		m_networkPingTimer += pRealEvt->m_frameTime;

		// update inputs

		CharacterControlContext *pCtx = m_pContext->get<CharacterControlContext>();

		SpaceShipGameControls *pCtrl = pCtx->getSpaceShipGameControls();

		static const float DeadZoneThreshold = 0.1f;
		static const float RollAcc = 2.0f;
		static const float RollDecc = 1.5f;
		static const float MaxRollSpeed = 1.5f;

		float prevRollAbs = abs(m_rollVel);
		float prevRollVel = m_rollVel;
		if (abs(pCtrl->m_roll) <= DeadZoneThreshold)
		{
			m_rollVel = pemath::sign(m_rollVel) * max(0.0f, prevRollAbs - RollDecc * pRealEvt->m_frameTime);
		}
		else
		{
			m_rollVel = pemath::clamp(-MaxRollSpeed, MaxRollSpeed, m_rollVel + pemath::sign(pCtrl->m_roll) * RollAcc * pRealEvt->m_frameTime);
		}

		float rollAcc = (m_rollVel - prevRollVel) / pRealEvt->m_frameTime;

		m_roll += m_rollVel * pRealEvt->m_frameTime;

		static const float MaxCamRoll = 0.75f;
		float desiredCamRoll = (m_rollVel / MaxRollSpeed) * MaxCamRoll;

		const static float MaxCamRollFromVel = 0.5f;

		float desiredCamRollVelAbs =  m_rollVel * MaxCamRollFromVel;
		float desiredCamRollVel = (desiredCamRoll - m_cameraRoll) / pRealEvt->m_frameTime;
		//pemath::sign(desiredCamRoll - m_cameraRoll) * min(desiredCamRollVelAbs, abs(desiredCamRoll - m_cameraRoll) / pRealEvt->m_frameTime);
		/*
		if (desiredCamRollVel > 0.0f ==  m_rollVel > 0.0f)
		{
			if (desiredCamRollVel > 0.0f)
			{
				if (desiredCamRollVel > m_rollVel * MaxCamRollFromVel)
					desiredCamRollVel = m_rollVel * MaxCamRollFromVel;
			}
			else
			{
				if (desiredCamRollVel < m_rollVel * MaxCamRollFromVel)
					desiredCamRollVel = m_rollVel * MaxCamRollFromVel;
			}

		}
		else
		{
			if (desiredCamRollVel > 0.0f)
			{
				if (desiredCamRollVel > -m_rollVel * MaxCamRollFromVel)
					desiredCamRollVel = -m_rollVel * MaxCamRollFromVel;
			}
			else
			{
				if (desiredCamRollVel < -m_rollVel * MaxCamRollFromVel)
					desiredCamRollVel = -m_rollVel * MaxCamRollFromVel;
			}
			//const static float MaxCamBackRollSpeed = 0.25f;
			//desiredCamRollVel = pemath::sign(desiredCamRollVel) * min(MaxCamBackRollSpeed, abs(desiredCamRollVel));
		}
		*/

		m_cameraRoll += desiredCamRollVel * pRealEvt->m_frameTime;

		//m_cameraRoll += m_rollVel * pRealEvt->m_frameTime;
		//
		//m_cameraRoll = pemath::clamp(-MaxCamRoll, MaxCamRoll, m_cameraRoll);

		Quaternion q(Vector3(0.0f, 0.0f, 1.0f), m_roll);

		pFirstSN->m_base = Matrix4x4(q);
	}

	SceneNode *pCamRollSN = pFirstSN->getFirstComponent<SceneNode>();
	
	// note we could have stored the camera reference in this object instead of searching for camera scene node
	if (CameraSceneNode *pCamSN = pCamRollSN->getFirstComponent<CameraSceneNode>())
	{
		static float x = 0.0f;
		static float y = 5.0f;
		static float z = -13.0f;

		pCamSN->m_base.loadIdentity();
		pCamSN->m_base.setPos(Vector3(x,y,z));

		static const float CameraRollFactor = 0.5f;
		static const float MaxCameraRoll = 0.5f;

		pCamRollSN->m_base.loadIdentity();
		pCamRollSN->m_base.rollLeft(m_cameraRoll);
	}


	if (!m_overriden)
	{
		/*
		if (m_time > 2.0f*PrimitiveTypes::Constants::c_Pi_F32)
		{
			m_time = 0;
			if (m_counter)
			{
				m_counter = 0;
				m_center = Vector2(0,0);
			}
			else
			{
				m_counter = 1;
				m_center = Vector2(10.0f, 0);
			}
		}
	    
		Vector3 pos = Vector3(m_center.m_x, 0, m_center.m_y);
		pos.m_x += (float)cos(m_time) * 5.0f * (m_counter ? -1.0f : 1.0f);
		pos.m_z += (float)sin(m_time) * 5.0f;
		pos.m_y = 0;
	    
		Vector3 fwrd;
		fwrd.m_x = -(float)sin(m_time)  * (m_counter ? -1.0f : 1.0f);
		fwrd.m_z = (float)cos(m_time);
		fwrd.m_y = 0;
	    
		Vector3 right;
		right.m_x = (float)cos(m_time) * (m_counter ? -1.0f : 1.0f) * (m_counter ? -1.0f : 1.0f);
		right.m_z = (float)sin(m_time) * (m_counter ? -1.0f : 1.0f);
		right.m_y = 0;
	   
        
		pFirstSN->m_base.setPos(m_spawnPos + pos);
		pFirstSN->m_base.setN(fwrd);
		pFirstSN->m_base.setU(right);
		*/
	}
	else
	{
		pFirstSN->m_base = m_transformOverride;
	}
    /*
	if (m_networkPingTimer > m_networkPingInterval)
	{
		// send client authoritative position event
		CharacterControl::Events::Event_MoveTank_C_to_S evt(*m_pContext);
		evt.m_transform = pFirstSN->m_base;

		ClientNetworkManager *pNetworkManager = (ClientNetworkManager *)(m_pContext->getNetworkManager());
		pNetworkManager->getNetworkContext().getEventManager()->scheduleEvent(&evt, m_pContext->getGameObjectManager(), true);

		m_networkPingTimer = 0.0f;
	}
	*/
}

void ClientSpaceShip::overrideTransform(Matrix4x4 &t)
{
	m_overriden = true;
	m_transformOverride = t;
}

void ClientSpaceShip::activate()
{
	m_active = true;

	// this function is called on client tank. since this is client tank and we have lcient authoritative movement
	// we need to register event handling for movement here
	//PE_REGISTER_EVENT_HANDLER(Event_Tank_Throttle, TankController::do_Tank_Throttle);
	//PE_REGISTER_EVENT_HANDLER(Event_Tank_Turn, TankController::do_Tank_Turn);

	PE::Handle hFisrtSN = getFirstComponentHandle<SceneNode>();
	if (!hFisrtSN.isValid())
	{
		assert(!"wrong setup. must have scene node referenced");
		return;
	}
	SceneNode *pFirstSN = hFisrtSN.getObject<SceneNode>();

	PE::Handle hCamRollParent("SceneNode", sizeof(SceneNode));
	SceneNode *pCamRollSN = new(hCamRollParent) SceneNode(*m_pContext, m_arena, hCamRollParent);
	pCamRollSN->addDefaultComponents();

	pFirstSN->addComponent(hCamRollParent);

	//create camera
	PE::Handle hCamera("Camera", sizeof(Camera));
	Camera *pCamera = new(hCamera) Camera(*m_pContext, m_arena, hCamera, hCamRollParent);
	pCamera->addDefaultComponents();
	CameraManager::Instance()->setCamera(CameraManager::VEHICLE, hCamera);

	CameraManager::Instance()->selectActiveCamera(CameraManager::VEHICLE);

	//disable default camera controls

	m_pContext->getDefaultGameControls()->setEnabled(false);

	m_pContext->get<CharacterControlContext>()->getTankGameControls()->setEnabled(false);

	m_pContext->get<CharacterControlContext>()->getSpaceShipGameControls()->setEnabled(true);
}


}
}
