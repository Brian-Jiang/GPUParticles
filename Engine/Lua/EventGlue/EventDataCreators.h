#ifndef __PYENGINE_2_0_LUA_GLUE_EVT_DATA_CREATORS_H__
#define __PYENGINE_2_0_LUA_GLUE_EVT_DATA_CREATORS_H__

// this file defines lua glue functions that create event data
// these functions will be called from lua to create event data


// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

// Outer-Engine includes
#include <assert.h>

#include "../../Lua/typedefs.h"

// Inter-Engine includes
#include "PrimeEngine/MemoryManagement/Handle.h"
#include "PrimeEngine/PrimitiveTypes/PrimitiveTypes.h"
#include "PrimeEngine/Events/Component.h"
#include "PrimeEngine/Utils/Array/Array.h"
#include "PrimeEngine/Logging/Log.h"
#include "PrimeEngine/Math/Matrix4x4.h"

#include "../typedefs.h"
namespace PE {
	namespace Components
	{
		struct LuaEnvironment;
	};

namespace LuaGlue
{

// Helpers
void debugStackObject(lua_State* luaVM, int offset);

void pushTableBuiltFromMatrix(lua_State* luaVM, const Matrix4x4 &m);
bool pushTableBuiltFromHandle(lua_State* luaVM, Handle h);
bool pushMethodLightUserData(lua_State* luaVM, Components::Component::HandlerMethod method);
bool pushStaticMethodLightUserData(lua_State* luaVM, Components::Component::StaticHandlerMethod method);
bool popHandleFromTableOnStackAndPopTable(lua_State* luaVM, Handle &h);
bool lookupFloat(lua_State* luaVM, const char *name, float &f);
bool lookupFloat3(lua_State* luaVM, const char *name, float &f0, float &f1, float &f2);
bool lookupFloat4(lua_State* luaVM, const char *name, float &f0, float &f1, float &f2, float &f3);

void pushString(lua_State* luaVM, const char *s);
void stringToHandle(PE::Components::LuaEnvironment *pLuaEnv, lua_State* luaVM, const char *pString);

void createTableOnTopOfStack(lua_State* luaVM);
PrimitiveTypes::UInt32 getTableSizeSlow(lua_State* luaVM); // for any table [0] = .., ['a'] = .., ..
PrimitiveTypes::UInt32 getTableSizeFast(lua_State* luaVM); // for indexed arrays [0], [1], ..

LuaReference storeReferenceToTop(lua_State* luaVM);

void putTableOnTopOfStackByReference(lua_State *luaVM, LuaReference ref);


void pushTableBuiltFromEvent(lua_State* luaVM, Events::Event *pEvt);

int l_outputDebugString(lua_State* luaVM);

void registerFunctions(lua_State* luaVM);

void pushNumber(lua_State* luaVM, PrimitiveTypes::Double value);
void pushPEUUID(lua_State* luaVM, const PEUUID &peuuid);
void pushInt32(lua_State* luaVM, PrimitiveTypes::Int32 v);

PEUUID readPEUUID(lua_State* luaVM, int tableIndex);
PrimitiveTypes::Int32 readInt32(lua_State* luaVM, int tableIndex);

int l_constructPEUUID(lua_State *luaVM);
int l_equalPEUUID(lua_State *luaVM);

int l_equalInt32(lua_State *luaVM);

}; // namespace LuaG;ue
}; // namespace PE


#endif
