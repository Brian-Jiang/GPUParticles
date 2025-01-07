#ifndef _CHARACTER_CONTROL_EVENTS_
#define _CHARACTER_CONTROL_EVENTS_

#include "PrimeEngine/Events/StandardEvents.h"

namespace CharacterControl
{
namespace Events
{
struct Event_CreateSoldierNPC : public PE::Events::Event_CREATE_MESH
{
	PE_DECLARE_CLASS(Event_CreateSoldierNPC);

	Event_CreateSoldierNPC(int &threadOwnershipMask): PE::Events::Event_CREATE_MESH(threadOwnershipMask){}
	// override SetLuaFunctions() since we are adding custom Lua interface
	static void SetLuaFunctions(PE::Components::LuaEnvironment *pLuaEnv, lua_State *luaVM);

	// Lua interface prefixed with l_
	static int l_Construct(lua_State* luaVM);

	int m_npcType;
	char m_gunMeshName[64];
	char m_gunMeshPackage[64];
	char m_patrolWayPoint[32];
};

struct Event_CreateTargetNPC : public PE::Events::Event_CREATE_MESH {
	PE_DECLARE_CLASS(Event_CreateTargetNPC);

	Event_CreateTargetNPC(int& threadOwnershipMask) : PE::Events::Event_CREATE_MESH(threadOwnershipMask) {}
	// override SetLuaFunctions() since we are adding custom Lua interface
	//static void SetLuaFunctions(PE::Components::LuaEnvironment* pLuaEnv, lua_State* luaVM);

	//// Lua interface prefixed with l_
	//static int l_Construct(lua_State* luaVM);

	//int m_npcType;
	//char m_gunMeshName[64];
	//char m_gunMeshPackage[64];
	//char m_patrolWayPoint[32];
};

struct Event_MoveTank_C_to_S : public PE::Events::Event, public PE::Networkable
{
	PE_DECLARE_CLASS(Event_MoveTank_C_to_S);
	PE_DECLARE_NETWORKABLE_CLASS

	Event_MoveTank_C_to_S(PE::GameContext &context);
	// Netoworkable:
	virtual int packCreationData(char *pDataStream);
	virtual int constructFromStream(char *pDataStream);


	// Factory function used by network
	static void *FactoryConstruct(PE::GameContext&, PE::MemoryArena);


	Matrix4x4 m_transform;
};


struct Event_MoveTank_S_to_C : public Event_MoveTank_C_to_S
{
	PE_DECLARE_CLASS(Event_MoveTank_S_to_C);
	PE_DECLARE_NETWORKABLE_CLASS

	Event_MoveTank_S_to_C(PE::GameContext &context);
	// Netoworkable:
	virtual int packCreationData(char *pDataStream);
	virtual int constructFromStream(char *pDataStream);


	// Factory function used by network
	static void *FactoryConstruct(PE::GameContext&, PE::MemoryArena);

	int m_clientTankId;
};


// tank input controls

struct Event_Tank_Throttle : public PE::Events::Event {
	PE_DECLARE_CLASS(Event_Tank_Throttle);

	Event_Tank_Throttle(){}
	virtual ~Event_Tank_Throttle(){}

	Vector3 m_relativeMove;
};

struct Event_Tank_Turn : public PE::Events::Event {
	PE_DECLARE_CLASS(Event_Tank_Turn);

	Event_Tank_Turn(){}
	virtual ~Event_Tank_Turn(){}

	Vector3 m_relativeRotate;
};

}; // namespace Events
}; // namespace CharacterControl

#endif
