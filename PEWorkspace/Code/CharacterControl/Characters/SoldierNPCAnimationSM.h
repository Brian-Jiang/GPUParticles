#ifndef _PE_SOLDIER_NPC_ANIMATION_SM_H_
#define _PE_SOLDIER_NPC_ANIMATION_SM_H_


#include "PrimeEngine/Events/Component.h"
#include "PrimeEngine/Scene/DefaultAnimationSM.h"


#include "../Events/Events.h"

namespace CharacterControl{

// events that can be sent to this state machine
namespace Events
{

// sent by movement state machine when a soldier has to stop
struct SoldierNPCAnimSM_Event_STOP : public PE::Events::Event {
	PE_DECLARE_CLASS(SoldierNPCAnimSM_Event_STOP);

	SoldierNPCAnimSM_Event_STOP() {}
};

// sent by movement state machine when a soldier has to walk
struct SoldierNPCAnimSM_Event_WALK : public PE::Events::Event {
	PE_DECLARE_CLASS(SoldierNPCAnimSM_Event_WALK);

	SoldierNPCAnimSM_Event_WALK() {}
};

struct SoldierNPCAnimSM_Event_Run : public PE::Events::Event {
	PE_DECLARE_CLASS(SoldierNPCAnimSM_Event_Run);

	SoldierNPCAnimSM_Event_Run() {}
};

// todo add events for shooting (sent by weapons state machine
struct SoldierNPCAnimSM_Event_StandShoot : public PE::Events::Event {
	PE_DECLARE_CLASS(SoldierNPCAnimSM_Event_StandShoot);

	SoldierNPCAnimSM_Event_StandShoot() {}
};

};

namespace Components {

struct SoldierNPCAnimationSM : public PE::Components::DefaultAnimationSM
{
	PE_DECLARE_CLASS(SoldierNPCAnimationSM);
	
	enum AnimId
	{
		NONE = -1,
		STAND = 20,
		WALK = 5,
		RUN = 18,
		STAND_AIM = 1,
		STAND_SHOOT = 16,
	};

	SoldierNPCAnimationSM(PE::GameContext &context, PE::MemoryArena arena, PE::Handle hMyself);

	// event handling
	virtual void addDefaultComponents();
	PE_DECLARE_IMPLEMENT_EVENT_HANDLER_WRAPPER(do_SoldierNPCAnimSM_Event_STOP)
	virtual void do_SoldierNPCAnimSM_Event_STOP(PE::Events::Event *pEvt);
	PE_DECLARE_IMPLEMENT_EVENT_HANDLER_WRAPPER(do_SoldierNPCAnimSM_Event_WALK)
	virtual void do_SoldierNPCAnimSM_Event_WALK(PE::Events::Event *pEvt);
	
	PE_DECLARE_IMPLEMENT_EVENT_HANDLER_WRAPPER(do_SoldierNPCAnimSM_Event_Run)
	virtual void do_SoldierNPCAnimSM_Event_Run(PE::Events::Event *pEvt);

	PE_DECLARE_IMPLEMENT_EVENT_HANDLER_WRAPPER(do_SoldierNPCAnimSM_Event_StandShoot)
	virtual void do_SoldierNPCAnimSM_Event_StandShoot(PE::Events::Event* pEvt);

	AnimId m_curId;
};

};
};


#endif


