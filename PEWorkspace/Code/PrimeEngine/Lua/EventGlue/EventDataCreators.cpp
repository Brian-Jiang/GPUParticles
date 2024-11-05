
// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

#include "PrimeEngine/FileSystem/FileWriter.h"
#include "EventDataCreators.h"
#include "PrimeEngine/Scene/Mesh.h"

#include "PrimeEngine/Geometry/SkeletonCPU/AnimationSetCPU.h"
#include "PrimeEngine/Lua/LuaEnvironment.h"


namespace PE {

namespace LuaGlue {

using namespace PE::Events;
using namespace PE::Components;

// need to call this function once to register glue functions within lua
void registerFunctions(lua_State* luaVM)
{
	
	lua_register(luaVM, "outputDebugString", l_outputDebugString);

	static const struct luaL_Reg l_peuuid[] = {
		{"l_constructPEUUID", l_constructPEUUID},
		{"l_equalPEUUID", l_equalPEUUID},
		{NULL, NULL} // sentinel
	};

	luaL_register(luaVM, "l_peuuid", l_peuuid);

	static const struct luaL_Reg l_int32[] = {
		{"l_equalInt32", l_equalInt32},
		{NULL, NULL} // sentinel
	};

	luaL_register(luaVM, "l_int32", l_int32);
}


void pushString(lua_State* luaVM, const char *s)
{
	lua_pushstring(luaVM, s);
}

void pushNumber(lua_State* luaVM, PrimitiveTypes::Double value)
{
	lua_pushnumber(luaVM, value);
}

void pushTableBuiltFromMatrix(lua_State* luaVM, const Matrix4x4 &m)
{
	lua_newtable(luaVM);

	for (PrimitiveTypes::UInt32 im = 0; im < 4; im++)
	{
		for (PrimitiveTypes::UInt32 in = 0; in < 4; in++)
		{
			lua_pushnumber(luaVM, im * 4 + in);   /* Push the table index */
			lua_pushnumber(luaVM, m.m[im][in]); /* Push the cell value */
			lua_rawset(luaVM, -3);
		}
	}
}

bool pushTableBuiltFromHandle(lua_State* luaVM, Handle h)
{
	lua_newtable(luaVM);

	// Handle is just 3 numbers
	lua_pushnumber(luaVM, 0); // index
	// push string to Lua 
	// important: when reading back this handle the name ptr will be set to 0, because strings returned from Lua will not be stroed in memory
	if (h.isValid())
		lua_pushstring(luaVM, h.m_dbgName); 
	else
		lua_pushstring(luaVM, NULL); 
	lua_rawset(luaVM, -3);

	lua_pushnumber(luaVM, 1); // index
	lua_pushnumber(luaVM, h.m_memoryPoolIndex); // value
	lua_rawset(luaVM, -3);

	lua_pushnumber(luaVM, 2); // index
	lua_pushnumber(luaVM, h.m_memoryBlockIndex); // value
	lua_rawset(luaVM, -3);

	lua_pushnumber(luaVM, 3); // index
	lua_pushlightuserdata(luaVM, h.m_cachedPtr); // value
	lua_rawset(luaVM, -3);

	return true;
}

bool pushMethodLightUserData(lua_State* luaVM, Components::Component::HandlerMethod method)
{
	void *ptr;
	memcpy(&ptr, &method, sizeof(Components::Component::HandlerMethod));

	lua_pushlightuserdata(luaVM, ptr);
	return true;
}

bool pushStaticMethodLightUserData(lua_State* luaVM, Components::Component::StaticHandlerMethod method)
{
	void *ptr;
	memcpy(&ptr, &method, sizeof(Components::Component::StaticHandlerMethod));

	lua_pushlightuserdata(luaVM, ptr);
	return true;
}

PrimitiveTypes::UInt32 getTableSizeSlow(lua_State* luaVM)
{
	PrimitiveTypes::UInt32 res = 0;
	// a table of queues exists on top

	 /* table is in the stack at index 't' */
	lua_pushnil(luaVM);  /* first key */
	while (lua_next(luaVM, -2) != 0)
	{
		res++;
		/* uses 'key' (at index -2) and 'value' (at index -1) */
		printf("%s - %s\n",
			lua_typename(luaVM, lua_type(luaVM, -2)),
			lua_typename(luaVM, lua_type(luaVM, -1)));

		/* removes 'value'; keeps 'key' for next iteration */
		lua_pop(luaVM, 1);
	}

	return res;
}

PrimitiveTypes::UInt32 getTableSizeFast(lua_State* luaVM)
{
	size_t res = lua_objlen(luaVM, -1);
	
	return res;
}


void debugStackObject(lua_State* luaVM, int offset)
{
	PEINFO("type: %d %s\n", int(lua_type(luaVM, offset)), lua_typename(luaVM, lua_type(luaVM, offset)));
	PEINFO("isnil: %d\n", int(lua_isnil(luaVM, offset)));
	PEINFO("istable: %d\n", int(lua_istable(luaVM, offset)));
	PEINFO("isfunction: %d\n", int(lua_isfunction(luaVM, offset)));
	PEINFO("isnumber: %d\n", int(lua_isnumber(luaVM, offset)));
}

bool popHandleFromTableOnStackAndPopTable(lua_State* luaVM, Handle &h)
{
    //dbg
	//int x = lua_gettop(luaVM);
	//bool b = lua_istable(luaVM, -1);
	//int a = lua_type(luaVM, -1);
	lua_pushnumber(luaVM, 0);
	lua_gettable(luaVM, -2); // get background[key]
	//const char* dHDbgName = lua_tostring(luaVM, -1);
	lua_pop(luaVM, 1);

	lua_pushnumber(luaVM, 1);
	lua_gettable(luaVM, -2); // get background[key]
	double dHMemoryPool = lua_tonumber(luaVM, -1);
	lua_pop(luaVM, 1);

	lua_pushnumber(luaVM, 2);
	lua_gettable(luaVM, -2); // get background[key]
	double dHMemortBlockIndex = lua_tonumber(luaVM, -1);
	lua_pop(luaVM, 1);

	lua_pushnumber(luaVM, 3);
	lua_gettable(luaVM, -2); // get background[key]
	void *ptr = 0;
	if (lua_isnumber(luaVM, -1))
		ptr = (void *)((int)(lua_tonumber(luaVM, -1)));
	else if (lua_islightuserdata(luaVM, -1))
		ptr = lua_touserdata(luaVM, -1);
	else
		assert(!"invalid type passed in for handle's 4th element");

	lua_pop(luaVM, 1);


	lua_pop(luaVM, 1); // pop table itself
	
	// important: when reading back this handle the name ptr will be set to 0, because strings returned from Lua will not be stored in memory
	h.m_dbgName = 0;
	h.m_memoryPoolIndex = (PrimitiveTypes::UInt32)(dHMemoryPool);
	h.m_memoryBlockIndex = (PrimitiveTypes::UInt32)(dHMemortBlockIndex);
	h.m_cachedPtr = ptr;
	if (!ptr)
		h.cachePointer(); // recreate pointer from memory pool, block indices

	return true;
}

bool lookupFloat(lua_State* luaVM, const char *name, float &f)
{
	lua_pushstring(luaVM, name);
	lua_gettable(luaVM, -2);
	f = float(lua_tonumber(luaVM, -1));
	lua_pop(luaVM, 1);

	return true;
}

bool lookupFloat3(lua_State* luaVM, const char *name, float &f0, float &f1, float &f2)
{
	lua_pushstring(luaVM, name);
	lua_gettable(luaVM, -2);

	lua_pushnumber(luaVM, 1);
	lua_gettable(luaVM, -2);
	f0 = float(lua_tonumber(luaVM, -1));
	lua_pop(luaVM, 1);

	lua_pushnumber(luaVM, 2);
	lua_gettable(luaVM, -2);
	f1 = float(lua_tonumber(luaVM, -1));
	lua_pop(luaVM, 1);

	lua_pushnumber(luaVM, 3);
	lua_gettable(luaVM, -2);
	f2 = float(lua_tonumber(luaVM, -1));
	lua_pop(luaVM, 1);


	lua_pop(luaVM, 1);

	return true;
}

bool lookupFloat4(lua_State* luaVM, const char *name, float &f0, float &f1, float &f2, float &f3)
{
	lua_pushstring(luaVM, name);
	lua_gettable(luaVM, -2);

	lua_pushnumber(luaVM, 1);
	lua_gettable(luaVM, -2);
	f0 = float(lua_tonumber(luaVM, -1));
	lua_pop(luaVM, 1);

	lua_pushnumber(luaVM, 2);
	lua_gettable(luaVM, -2);
	f1 = float(lua_tonumber(luaVM, -1));
	lua_pop(luaVM, 1);

	lua_pushnumber(luaVM, 3);
	lua_gettable(luaVM, -2);
	f2 = float(lua_tonumber(luaVM, -1));
	lua_pop(luaVM, 1);
	
	lua_pushnumber(luaVM, 4);
	lua_gettable(luaVM, -2);
	f3 = float(lua_tonumber(luaVM, -1));
	lua_pop(luaVM, 1);


	lua_pop(luaVM, 1);

	return true;
}


void stringToHandle(LuaEnvironment *pLuaEnv, lua_State* luaVM, const char *pString)
{
	int x = lua_gettop(luaVM);
	bool b = lua_istable(luaVM, -1);
	int a = lua_type(luaVM, -1);

	pLuaEnv->prepFunctionCall("stringAsHandle");
	pushString(luaVM, pString);

	x = lua_gettop(luaVM);
	b = lua_istable(luaVM, -1);
	a = lua_type(luaVM, -1);

	pLuaEnv->callPreparedFunction(1, 1, 0);

	x = lua_gettop(luaVM);
	b = lua_istable(luaVM, -1);
	a = lua_type(luaVM, -1);
}



int l_outputDebugString(lua_State* luaVM)
{
	const char * s = lua_tostring(luaVM, -1);
	lua_pop(luaVM, 1);

	if (StringOps::length(s) < 512)
	{
		PEINFOSTR(s);
	}
	else
	{
		PEINFO("PE: ERROR: l_outputDebugString: Cant't output more than 512 characters!\n");
	}

	return 0;
}

void pushPEUUID(lua_State* luaVM, const PEUUID &peuuid)
{
	/*PEUUID *p = (PEUUID *)(lua_newuserdata(luaVM, sizeof(PEUUID)));
	memcpy(p, &peuuid, sizeof(PEUUID));*/
	lua_pushlstring(luaVM, (const char *)(&peuuid), sizeof(PEUUID));
}

void pushInt32(lua_State* luaVM, PrimitiveTypes::Int32 v)
{
	lua_pushlstring(luaVM, (const char *)(&v), sizeof(PrimitiveTypes::Int32));
}

PEUUID readPEUUID(lua_State* luaVM, int tableIndex)
{
	PEUUID *p0;

	/*p0 = (PEUUID *)(lua_touserdata(luaVM, -2));
	*/
	size_t len = 0;
	
	p0 = (PEUUID *)(lua_tolstring(luaVM, tableIndex, &len));
	assert(len == sizeof(PEUUID));
	return *p0;
}

PrimitiveTypes::Int32 readInt32(lua_State* luaVM, int tableIndex)
{
	PrimitiveTypes::Int32 *p;

	size_t len = 0;
	
	p = (PrimitiveTypes::Int32 *)(lua_tolstring(luaVM, tableIndex, &len));
	assert(len == sizeof(PrimitiveTypes::Int32));
	return *p;
}

int l_constructPEUUID(lua_State *luaVM)
{
	// lua arguments are passed in reverse order
	PrimitiveTypes::UInt32 v0, v1, v2, v3;

	v0 = (PrimitiveTypes::UInt32)(lua_tonumber(luaVM, -4));
	v1 = (PrimitiveTypes::UInt32)(lua_tonumber(luaVM, -3));
	v2 = (PrimitiveTypes::UInt32)(lua_tonumber(luaVM, -2));
	v3 = (PrimitiveTypes::UInt32)(lua_tonumber(luaVM, -1));
	lua_pop(luaVM, 4); // pop all 4 args

	// put the PEUUID on stack as user data

	PEUUID peuuid;
	peuuid.set(v0, v1, v2, v3);

	pushPEUUID(luaVM, peuuid);
	
	return 1; // returning one value = PEUUID
}

int l_equalPEUUID(lua_State *luaVM)
{
	PEUUID p0, p1;
	p0 = readPEUUID(luaVM, -2);
	p1 = readPEUUID(luaVM, -1);
	
	PrimitiveTypes::Bool res = p0.isEqualTo(p1);

	lua_pop(luaVM, 2);

	lua_pushboolean(luaVM, res);
	
	return 1; // returning 1 result = whether two PEUUIDs are equal
}

int l_equalInt32(lua_State *luaVM)
{
	PrimitiveTypes::Int32 v0, v1;
	v0 = readInt32(luaVM, -2);
	v1 = readInt32(luaVM, -1);
	
	PrimitiveTypes::Bool res = v0 == v1;

	lua_pop(luaVM, 2);

	lua_pushboolean(luaVM, res);
	
	return 1; // returning 1 result = whether two PEUUIDs are equal
}

void pushTableBuiltFromEvent(lua_State* luaVM, Events::Event *pEvt)
{
	lua_newtable(luaVM);
	lua_pushstring(luaVM, "evtClassId"); // index

	pushInt32(luaVM, pEvt->getClassId());
	
	lua_rawset(luaVM, -3);
}

void createTableOnTopOfStack(lua_State* luaVM)
{
	lua_newtable(luaVM);
	int type = lua_type(luaVM, -1);
	assert(type == LUA_TTABLE);
}

LuaReference storeReferenceToTop(lua_State* luaVM)
{
	return luaL_ref(luaVM, LUA_REGISTRYINDEX);
}

void putTableOnTopOfStackByReference(lua_State *luaVM, LuaReference ref)
{
	lua_rawgeti(luaVM, LUA_REGISTRYINDEX, ref);
	assert(lua_type(luaVM, -1) == LUA_TTABLE);
	assert(lua_istable(luaVM, -1));
}


};
}; // namespace PE