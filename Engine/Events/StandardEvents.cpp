
#include "StandardEvents.h"

#include "PrimeEngine/Lua/LuaEnvironment.h"
#include "PrimeEngine/Lua/EventGlue/EventDataCreators.h"

#include "PrimeEngine/Networking/StreamManager.h"

namespace PE {
namespace Events {
	PE_IMPLEMENT_CLASS1(Event_UPDATE, Event);
	PE_IMPLEMENT_CLASS1(Event_SCENE_GRAPH_UPDATE, Event);
	PE_IMPLEMENT_CLASS1(Event_PRE_RENDER_needsRC, Event);
	PE_IMPLEMENT_CLASS1(Event_GATHER_DRAWCALLS, Event);
	PE_IMPLEMENT_CLASS1(Event_GATHER_DRAWCALLS_Z_ONLY, Event);
	PE_IMPLEMENT_CLASS1(Event_CALCULATE_TRANSFORMATIONS, Event);
	PE_IMPLEMENT_CLASS1(Event_PRE_GATHER_DRAWCALLS, Event);

//////////////////////////////////////////////////////////////////////////
// Event_POP_SHADERS
//////////////////////////////////////////////////////////////////////////

PE_IMPLEMENT_CLASS1(Event_POP_SHADERS, Event);

void Event_POP_SHADERS::SetLuaFunctions(PE::Components::LuaEnvironment *pLuaEnv, lua_State *luaVM)
{
	static const struct luaL_Reg l_Event_POP_SHADERS[] = {
		{"Construct", l_Construct},
		{NULL, NULL} // sentinel
	};

	// register the functions in current lua table which is the table for Event_POP_SHADERS
	luaL_register(luaVM, 0, l_Event_POP_SHADERS);
}

// this function is called form Lua whenever Lua wants to create an event
int Event_POP_SHADERS::l_Construct(lua_State* luaVM)
{
	Handle h("EVENT", sizeof(Event_POP_SHADERS));
	/*Event_POP_SHADERS *pEvt = */new(h) Event_POP_SHADERS;

	LuaGlue::pushTableBuiltFromHandle(luaVM, h); 
	return 1;
}

//////////////////////////////////////////////////////////////////////////
// Event_CHANGE_TO_DEBUG_SHADER
//////////////////////////////////////////////////////////////////////////

PE_IMPLEMENT_CLASS1(Event_CHANGE_TO_DEBUG_SHADER, Event);

void Event_CHANGE_TO_DEBUG_SHADER::SetLuaFunctions(PE::Components::LuaEnvironment *pLuaEnv, lua_State *luaVM)
{
	static const struct luaL_Reg l_Event_CHANGE_TO_DEBUG_SHADER[] = {
		{"Construct", l_Construct},
		{NULL, NULL} // sentinel
	};

	// register the functions in current lua table which is the table for Event_CHANGE_TO_DEBUG_SHADER
	luaL_register(luaVM, 0, l_Event_CHANGE_TO_DEBUG_SHADER);
}

// this function is called form Lua whenever Lua wants to create an event
int Event_CHANGE_TO_DEBUG_SHADER::l_Construct(lua_State* luaVM)
{
	Handle h("EVENT", sizeof(Event_CHANGE_TO_DEBUG_SHADER));
	/*Event_CHANGE_TO_DEBUG_SHADER *pEvt = */new(h) Event_CHANGE_TO_DEBUG_SHADER;

	LuaGlue::pushTableBuiltFromHandle(luaVM, h); 
	return 1;
}

//////////////////////////////////////////////////////////////////////////

PE_IMPLEMENT_CLASS1(Event_CLOSED_WINDOW, Event);

//////////////////////////////////////////////////////////////////////////

PE_IMPLEMENT_CLASS1(Event_PLAY_ANIMATION, Event);

void Event_PLAY_ANIMATION::SetLuaFunctions(PE::Components::LuaEnvironment *pLuaEnv, lua_State *luaVM)
{
	static const struct luaL_Reg l_Event_PLAY_ANIMATION[] = {
		{"Construct", l_Construct},
		{NULL, NULL} // sentinel
	};

	// register the functions in current lua table which is the table for Event_PLAY_ANIMATION
	luaL_register(luaVM, 0, l_Event_PLAY_ANIMATION);
}

// this function is called form Lua whenever Lua wants to create this event
int Event_PLAY_ANIMATION::l_Construct(lua_State* luaVM)
{
	Handle h("EVENT", sizeof(Event_PLAY_ANIMATION));
	Event_PLAY_ANIMATION *pEvt = new(h) Event_PLAY_ANIMATION;

	// read single number
	pEvt->m_animSetIndex = (PrimitiveTypes::Int32)(lua_tonumber(luaVM, -2));
	pEvt->m_animIndex = (PrimitiveTypes::Int32)(lua_tonumber(luaVM, -1));
	lua_pop(luaVM, 2);

	LuaGlue::pushTableBuiltFromHandle(luaVM, h); 
	return 1;
}

//////////////////////////////////////////////////////////////////////////

PE_IMPLEMENT_CLASS1(Event_ANIMATION_ENDED, Event);

//////////////////////////////////////////////////////////////////////////

PE_IMPLEMENT_CLASS1(Event_IK_REPORT, Event);

//////////////////////////////////////////////////////////////////////////
// Event_CREATE_LIGHT
//////////////////////////////////////////////////////////////////////////

PE_IMPLEMENT_CLASS1(Event_CREATE_LIGHT, Event);

// override SetLuaFunctions() since we are adding custom Lua interface
void Event_CREATE_LIGHT::SetLuaFunctions(PE::Components::LuaEnvironment *pLuaEnv, lua_State *luaVM)
{
	static const struct luaL_Reg l_Event_CREATE_LIGHT[] = {
		{"Construct", l_Construct},
		{NULL, NULL} // sentinel
	};

	// register the functions in current lua table which is the table for Event_CREATE_LIGHT
	luaL_register(luaVM, 0, l_Event_CREATE_LIGHT);
}

// Lua interface prefixed with l_
int Event_CREATE_LIGHT::l_Construct(lua_State* luaVM)
{
	Handle h("EVENT", sizeof(Event_CREATE_LIGHT));
	Event_CREATE_LIGHT *pEvt = new(h) Event_CREATE_LIGHT;

	int numArgs, iNumArgs;
	numArgs = iNumArgs = 32;

	Vector3 pos, u, v, n, attenuation;
	Vector4 diffuse, spec, ambient;
	float spotPower, range;
	bool isShadowCaster;
	
	float positionFactor = 1.0f / 100.0f;

	pos.m_x = (float)lua_tonumber(luaVM, -iNumArgs--) * positionFactor;
	pos.m_y = (float)lua_tonumber(luaVM, -iNumArgs--) * positionFactor;
	pos.m_z = (float)lua_tonumber(luaVM, -iNumArgs--) * positionFactor;

	u.m_x = (float)lua_tonumber(luaVM, -iNumArgs--);
	u.m_y = (float)lua_tonumber(luaVM, -iNumArgs--);
	u.m_z = (float)lua_tonumber(luaVM, -iNumArgs--);

	v.m_x = (float)lua_tonumber(luaVM, -iNumArgs--);
	v.m_y = (float)lua_tonumber(luaVM, -iNumArgs--);
	v.m_z = (float)lua_tonumber(luaVM, -iNumArgs--);

	n.m_x = (float)lua_tonumber(luaVM, -iNumArgs--);
	n.m_y = (float)lua_tonumber(luaVM, -iNumArgs--);
	n.m_z = (float)lua_tonumber(luaVM, -iNumArgs--);

	const char* typeStr = lua_tostring(luaVM, -iNumArgs--);

	diffuse.m_x = (float)lua_tonumber(luaVM, -iNumArgs--);
	diffuse.m_y = (float)lua_tonumber(luaVM, -iNumArgs--);
	diffuse.m_z = (float)lua_tonumber(luaVM, -iNumArgs--);
	diffuse.m_w = (float)lua_tonumber(luaVM, -iNumArgs--);

	spec.m_x = (float)lua_tonumber(luaVM, -iNumArgs--);
	spec.m_y = (float)lua_tonumber(luaVM, -iNumArgs--);
	spec.m_z = (float)lua_tonumber(luaVM, -iNumArgs--);
	spec.m_w = (float)lua_tonumber(luaVM, -iNumArgs--);

	ambient.m_x = (float)lua_tonumber(luaVM, -iNumArgs--);
	ambient.m_y = (float)lua_tonumber(luaVM, -iNumArgs--);
	ambient.m_z = (float)lua_tonumber(luaVM, -iNumArgs--);
	ambient.m_w = (float)lua_tonumber(luaVM, -iNumArgs--);

	attenuation.m_x = (float)lua_tonumber(luaVM, -iNumArgs--);
	attenuation.m_y = (float)lua_tonumber(luaVM, -iNumArgs--);
	attenuation.m_z = (float)lua_tonumber(luaVM, -iNumArgs--);

	spotPower = (float)lua_tonumber(luaVM, -iNumArgs--);
	range = (float)lua_tonumber(luaVM, -iNumArgs--);
	isShadowCaster = lua_toboolean(luaVM, -iNumArgs--) != 0;

	pEvt->m_peuuid = LuaGlue::readPEUUID(luaVM, -iNumArgs--);

	
	lua_pop(luaVM, numArgs); //Second arg is a count of how many to pop

	pEvt->m_pos = pos;
	pEvt->m_u = u;
	pEvt->m_v = v;
	pEvt->m_n = n;

	pEvt->m_diffuse = diffuse;
	pEvt->m_spec = spec;
	pEvt->m_ambient = ambient;
	pEvt->m_att = attenuation;
	pEvt->m_spotPower = spotPower;
	pEvt->m_range = range;
	pEvt->m_isShadowCaster = isShadowCaster;

	if (StringOps::strcmp(typeStr, "directional") == 0)
	{
		pEvt->m_type = 1;
	} else if (StringOps::strcmp(typeStr, "spot") == 0)
	{
		pEvt->m_type = 2;
	}
	
	LuaGlue::pushTableBuiltFromHandle(luaVM, h); 

	return 1;
}


//////////////////////////////////////////////////////////////////////////
// Event_CREATE_MESH
//////////////////////////////////////////////////////////////////////////

PE_IMPLEMENT_CLASS1(Event_CREATE_MESH, Event);

void Event_CREATE_MESH::SetLuaFunctions(PE::Components::LuaEnvironment *pLuaEnv, lua_State *luaVM)
{
	static const struct luaL_Reg l_Event_CREATE_MESH[] = {
		{"Construct", l_Construct},
		{NULL, NULL} // sentinel
	};

	// register the functions in current lua table which is the table for Event_CREATE_MESH
	luaL_register(luaVM, 0, l_Event_CREATE_MESH);
}

// this function is called form lua whenever Lua wants to create an event
int Event_CREATE_MESH::l_Construct(lua_State* luaVM)
{
	Handle h("EVENT", sizeof(Event_CREATE_MESH));
	PE::GameContext *pContext = (PE::GameContext*)(lua_touserdata(luaVM, -16));
	
	Event_CREATE_MESH *pEvt = new(h) Event_CREATE_MESH(pContext->m_gameThreadThreadOwnershipMask);

	const char* name = lua_tostring(luaVM, -15);
	const char* package = lua_tostring(luaVM, -14);

	float positionFactor = 1.0f / 100.0f;
	Vector3 pos, u, v, n;
	pos.m_x = (float)lua_tonumber(luaVM, -13) * positionFactor;
	pos.m_y = (float)lua_tonumber(luaVM, -12) * positionFactor;
	pos.m_z = (float)lua_tonumber(luaVM, -11) * positionFactor;

	u.m_x = (float)lua_tonumber(luaVM, -10);
	u.m_y = (float)lua_tonumber(luaVM, -9);
	u.m_z = (float)lua_tonumber(luaVM, -8);

	v.m_x = (float)lua_tonumber(luaVM, -7);
	v.m_y = (float)lua_tonumber(luaVM, -6);
	v.m_z = (float)lua_tonumber(luaVM, -5);

	n.m_x = (float)lua_tonumber(luaVM, -4);
	n.m_y = (float)lua_tonumber(luaVM, -3);
	n.m_z = (float)lua_tonumber(luaVM, -2);

	pEvt->m_peuuid = LuaGlue::readPEUUID(luaVM, -1);

	// set data values
	StringOps::writeToString(name, pEvt->m_meshFilename, 255);
	StringOps::writeToString(package, pEvt->m_package, 255);

	lua_pop(luaVM, 15); //Second arg is a count of how many to pop

	pEvt->hasCustomOrientation = true;

	pEvt->m_pos = pos;
	pEvt->m_u = u;
	pEvt->m_v = v;
	pEvt->m_n = n;
	pEvt->hasCustomOrientation = true;
	LuaGlue::pushTableBuiltFromHandle(luaVM, h); 

	return 1;
}

//////////////////////////////////////////////////////////////////////////
// Event_CREATE_SKELETON
//////////////////////////////////////////////////////////////////////////

PE_IMPLEMENT_CLASS1(Event_CREATE_SKELETON, Event);

void Event_CREATE_SKELETON::SetLuaFunctions(PE::Components::LuaEnvironment *pLuaEnv, lua_State *luaVM)
{
	static const struct luaL_Reg l_Event_CREATE_SKELETON[] = {
		{"Construct", l_Construct},
		{NULL, NULL} // sentinel
	};

	// register the functions in current lua table which is the table for Event_CREATE_SKELETON
	luaL_register(luaVM, 0, l_Event_CREATE_SKELETON);
}

// this function is called form lua whenever Lua wants to create an event
int Event_CREATE_SKELETON::l_Construct(lua_State* luaVM)
{
	Handle h("EVENT", sizeof(Event_CREATE_SKELETON));
	
	// get arguments from stack
	PE::GameContext *pContext = (PE::GameContext*)(lua_touserdata(luaVM, -16));

	Event_CREATE_SKELETON *pEvt = new(h) Event_CREATE_SKELETON(pContext->m_gameThreadThreadOwnershipMask);

	const char* name = lua_tostring(luaVM, -15);
	const char* package = lua_tostring(luaVM, -14);
	
	float positionFactor = 1.0f / 100.0f;

	Vector3 playerPos, u, v, n;
	playerPos.m_x = (float)lua_tonumber(luaVM, -13) * positionFactor;
	playerPos.m_y = (float)lua_tonumber(luaVM, -12) * positionFactor;
	playerPos.m_z = (float)lua_tonumber(luaVM, -11) * positionFactor;

	u.m_x = (float)lua_tonumber(luaVM, -10); u.m_y = (float)lua_tonumber(luaVM, -9); u.m_z = (float)lua_tonumber(luaVM, -8);
	v.m_x = (float)lua_tonumber(luaVM, -7); v.m_y = (float)lua_tonumber(luaVM, -6); v.m_z = (float)lua_tonumber(luaVM, -5);
	n.m_x = (float)lua_tonumber(luaVM, -4); n.m_y = (float)lua_tonumber(luaVM, -3); n.m_z = (float)lua_tonumber(luaVM, -2);

	pEvt->m_peuuid = LuaGlue::readPEUUID(luaVM, -1);
	pEvt->hasCustomOrientation = true;

	// set data values
	StringOps::writeToString(name, pEvt->m_skelFilename, 255);
	StringOps::writeToString(package, pEvt->m_package, 255);

	pEvt->hasCustomOrientation = true;
	pEvt->m_pos = playerPos;
	pEvt->m_u = u;
	pEvt->m_v = v;
	pEvt->m_n = n;

	lua_pop(luaVM, 15); //Second arg is a count of how many to pop

	LuaGlue::pushTableBuiltFromHandle(luaVM, h); 

	return 1;
}


//////////////////////////////////////////////////////////////////////////
// Event_CREATE_ANIM_SET
//////////////////////////////////////////////////////////////////////////

PE_IMPLEMENT_CLASS1(Event_CREATE_ANIM_SET, Event);

void Event_CREATE_ANIM_SET::SetLuaFunctions(PE::Components::LuaEnvironment *pLuaEnv, lua_State *luaVM)
{
	static const struct luaL_Reg l_Event_CREATE_ANIM_SET[] = {
		{"Construct", l_Construct},
		{NULL, NULL} // sentinel
	};

	// register the functions in current lua table which is the table for Event_CREATE_ANIM_SET
	luaL_register(luaVM, 0, l_Event_CREATE_ANIM_SET);
}

// this function is called form lua whenever Lua wants to create an event
int Event_CREATE_ANIM_SET::l_Construct(lua_State* luaVM)
{
	Handle h("EVENT", sizeof(Event_CREATE_ANIM_SET));

	// get arguments from stack
	PE::GameContext *pContext = (PE::GameContext*)(lua_touserdata(luaVM, -4));

	Event_CREATE_ANIM_SET *pEvt = new(h) Event_CREATE_ANIM_SET(pContext->m_gameThreadThreadOwnershipMask);

	const char* name = lua_tostring(luaVM, -3);
	const char* package = lua_tostring(luaVM, -2);

	pEvt->m_peuuid = LuaGlue::readPEUUID(luaVM, -1);

	// set data values
	StringOps::writeToString(name, pEvt->animSetFilename, 255);
	StringOps::writeToString(package, pEvt->m_package, 255);

	lua_pop(luaVM, 3); //Second arg is a count of how many to pop

	LuaGlue::pushTableBuiltFromHandle(luaVM, h); 

	return 1;
}


PE_IMPLEMENT_CLASS1(Event_CREATE_NAVGRID, Event);
PE_IMPLEMENT_CLASS1(Event_PHYSICS_END, Event);
PE_IMPLEMENT_CLASS1(Event_PHYSICS_START, Event);
PE_IMPLEMENT_CLASS1(Event_VORTEX_ENDED, Event);

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////

PE_IMPLEMENT_CLASS1(Event_MOVE, Event);

void Event_MOVE::SetLuaFunctions(PE::Components::LuaEnvironment *pLuaEnv, lua_State *luaVM)
{
	static const struct luaL_Reg l_Event_MOVE[] = {
		{"Construct", l_Construct},
		{NULL, NULL} // sentinel
	};

	// register the functions in current lua table which is the table for Event_MOVE
	luaL_register(luaVM, 0, l_Event_MOVE);
}

// this function is called form lua whenever Lua wants to create an event
int Event_MOVE::l_Construct(lua_State* luaVM)
{
	Handle h("EVENT", sizeof(Event_MOVE));
	Event_MOVE *pEvt = new(h) Event_MOVE;

	Vector3 dir;
	dir.m_x = (float)lua_tonumber(luaVM, -3);
	dir.m_y = (float)lua_tonumber(luaVM, -2);
	dir.m_z = (float)lua_tonumber(luaVM, -1);

	lua_pop(luaVM, 3); //Second arg is a count of how many to pop

	// set data values
	pEvt->m_dir = dir;
	LuaGlue::pushTableBuiltFromHandle(luaVM, h); 

	return 1;
}

PE_IMPLEMENT_CLASS1(Event_SET_DEBUG_TARGET_HANDLE, Event);


void Event_SET_DEBUG_TARGET_HANDLE::SetLuaFunctions(PE::Components::LuaEnvironment *pLuaEnv, lua_State *luaVM)
{
	static const struct luaL_Reg l_Event_SET_DEBUG_TARGET_HANDLE[] = {
		{"Construct", l_Construct},
		{NULL, NULL} // sentinel
	};

	// register the functions in current lua table which is the table for Event_MOVE
	luaL_register(luaVM, 0, l_Event_SET_DEBUG_TARGET_HANDLE);
}

// this function is called form lua whenever Lua wants to create this event
int Event_SET_DEBUG_TARGET_HANDLE::l_Construct(lua_State* luaVM)
{
	Handle h("EVENT", sizeof(Event_SET_DEBUG_TARGET_HANDLE));
	Event_SET_DEBUG_TARGET_HANDLE *pEvt = new(h) Event_SET_DEBUG_TARGET_HANDLE;
	
	// arguments are on the stack in reverse order
	int evtClassId = (int)(lua_tonumber(luaVM, -1));
	lua_pop(luaVM, 1);

	Handle dbgTarget;
	LuaGlue::popHandleFromTableOnStackAndPopTable(luaVM, dbgTarget);

	
	// set data values
	pEvt->m_hDebugTarget = dbgTarget;
	pEvt->m_debugEvent = evtClassId;

	LuaGlue::pushTableBuiltFromHandle(luaVM, h); 

	return 1;
}


PE_IMPLEMENT_CLASS1(Event_CONSTRUCT_SOUND, Event);
PE_IMPLEMENT_CLASS1(Event_ADDED_AS_COMPONENT, Event);
PE_IMPLEMENT_CLASS1(Event_CHARACTER_HIT_BY_MELEE, Event);

PE_IMPLEMENT_CLASS1(Event_SERVER_CLIENT_CONNECTION_ACK, Event);
Event_SERVER_CLIENT_CONNECTION_ACK::Event_SERVER_CLIENT_CONNECTION_ACK(PE::GameContext &context)
: Networkable(context, this)
, m_clientId(-1)
{

}

Event_SERVER_CLIENT_CONNECTION_ACK::~Event_SERVER_CLIENT_CONNECTION_ACK()
{
	assert("Quick test");
}


void *Event_SERVER_CLIENT_CONNECTION_ACK::FactoryConstruct(PE::GameContext& context, PE::MemoryArena arena)
{
	Event_SERVER_CLIENT_CONNECTION_ACK *pEvt = new (arena) Event_SERVER_CLIENT_CONNECTION_ACK(context);
	return pEvt;
}

int Event_SERVER_CLIENT_CONNECTION_ACK::packCreationData(char *pDataStream)
{
	int written = 0;
	written += PE::Components::StreamManager::WriteInt32(m_clientId, pDataStream);
	return written;
}

int Event_SERVER_CLIENT_CONNECTION_ACK::constructFromStream(char *pDataStream)
{
	int read = 0;
	read += PE::Components::StreamManager::ReadInt32(&pDataStream[read], m_clientId);
	return read;
}

PE_IMPLEMENT_CLASS1(Event_Update_Physics, Event);

};
};

