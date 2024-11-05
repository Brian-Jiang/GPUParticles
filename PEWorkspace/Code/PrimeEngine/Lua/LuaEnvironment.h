#ifndef __PYENGINE_2_0_LUA_COMPONENT_H__
#define __PYENGINE_2_0_LUA_COMPONENT_H__

// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

// Outer-Engine includes
#include <assert.h>

// Lua Stuff
extern "C" {
#include "../../lua_dist/src/lua.h"
#include "../../lua_dist/src/lualib.h"
#include "../../lua_dist/src/lauxlib.h"
#include "../../luasocket_dist/src/luasocket.h"
}

// Inter-Engine includes
#include "PrimeEngine/MemoryManagement/Handle.h"
#include "PrimeEngine/PrimitiveTypes/PrimitiveTypes.h"
#include "../Events/Component.h"
#include "../Utils/Array/Array.h"

#include "PrimeEngine/Utils/Networkable.h"

namespace PE {
namespace Components {
struct LuaEnvironment : public Component, public Networkable
{
	PE_DECLARE_CLASS(LuaEnvironment);
	PE_DECLARE_NETWORKABLE_CLASS

	enum LuaThread
	{
		LuaThread_Game,
		LuaThread_Physics,
		LuaThread_Draw,
		LuaThread_Network,
		LuaThread_Count
	};

	// Constructor -------------------------------------------------------------
	LuaEnvironment(PE::GameContext &context, PE::MemoryArena arena, Handle hMyself);

	virtual void run() = 0;

	virtual ~LuaEnvironment(){}

	void closeStates();

	virtual void addDefaultComponents();

	PE_DECLARE_IMPLEMENT_EVENT_HANDLER_WRAPPER(do_UPDATE);
	virtual void do_UPDATE(Events::Event *pEvt);

	void stringToHandle(lua_State* luaVM, const char *pString);


	void pushString(const char *pString);
	void pushNumber(PrimitiveTypes::Double value);
	void pushPEUUID(const PEUUID &peuuid);
	void pushInt32(PrimitiveTypes::Int32 v, lua_State *pL);
	void pushTableBuiltFromEvent(Events::Event *pEvt);
	void pushNewTableAsField(PEUUID &key);
	void pushNewTableAsFieldKeyedByInt32(PrimitiveTypes::Int32);
	void pushNewTableAsFieldKeyedByString(const char * key, bool leaveOnTop = false);
	void pushHandle(Handle &h);
	void pushHandleAsFieldAndSet(PEUUID &key, Handle &h);
	void pushInt32AsFieldAndSet(const char*key, PrimitiveTypes::Int32 v, lua_State *pL = NULL);
	static bool pushHandleAsNextArrayElementIfNotInArray(lua_State *L, Handle &h);
	static bool pushMethodAsNextArrayElementIfNotInArray(lua_State *L, Component::HandlerMethod method);
	static bool pushStaticMethodAsNextArrayElementIfNotInArray(lua_State *L, Component::StaticHandlerMethod method);

	static void popHandleFromTableOnStackAndPopTable(lua_State *L, Handle &h);

	static LuaGlue::LuaReference createTableOnTopOfStackAndStoreReference(lua_State *pL);
	static void putTableOnTopOfStackByReference(LuaGlue::LuaReference ref, lua_State *pL);

	bool checkTableValueByInt32KeyExists(int key);
	bool checkTableValueByPEUUIDFieldExists(PEUUID &key);
	
	// sets a value for a table currently on stack
	void setTableValue(const char *key, double val, lua_State *pL = NULL);

	void pop();

	PrimitiveTypes::Double popNumber();

	void passEventToQueue(Events::Event *pEvt);

	void iterateOverTableAndGetEventTypes(Array<PrimitiveTypes::Int32> &arr);

	PrimitiveTypes::Bool runScript(const char *pFname);
	PrimitiveTypes::Bool runScriptDefaultPath(const char *pFname);
	PrimitiveTypes::Bool runScriptWorkspacePath(const char *pFname);

	PrimitiveTypes::Bool runString(const char *pCommand);

	PrimitiveTypes::Int32 prepFunctionCall(const char *fname);
	PrimitiveTypes::Int32 prepModuleFunctionCall(const char *module, const char *fname);

	PrimitiveTypes::Int32 callPreparedFunction(int nargs, int nresults, int errfunc);

	PrimitiveTypes::Int32 runFunction(const char *fname, int nresults, int errfunc);

	virtual void registerInitialLibrariesFunctions();
	static int lua_getGameObjectManagerHandle(lua_State* luaVM);
	static int l_getLastAddedGameObjectHandle(lua_State* luaVM);

	static int l_getEffectManagerHandle(lua_State* luaVM);

	static int lua_getRootSceneNodeHandle(lua_State* luaVM);

	static int l_getGameProjRoot(lua_State* luaVM);
	static int l_getLuaCommandServerPort(lua_State* luaVM);
	static int l_setCreatedLuaCommandServerPort(lua_State* luaVM);
	static int l_getPathDelimeter(lua_State* luaVM);
	static int l_convertUserdataPointerToNumber(lua_State *luaVM);
	static int l_findluaFiles(lua_State* luaVM);
	static void findluaFilesRecursive(PE::GameContext &context, PE::MemoryArena arena, PEString &path, PEString &pattern, lua_State* luaVM, PrimitiveTypes::UInt32 &curIndex);

	static int l_getClassNameByClassId(lua_State *luaVM);

	//////////////////////////////////////////////////////////////////////////
	// LuaEnvironment Lua Interface
	//////////////////////////////////////////////////////////////////////////
	//
	static void SetLuaFunctions(PE::Components::LuaEnvironment *pLuaEnv, lua_State *luaVM);
	//
	static int l_MemoryReport(lua_State* luaVM);
	//
	//////////////////////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////////////////////
	// PE class registration utilities
	//////////////////////////////////////////////////////////////////////////
	//
	void StartRootRegistrationTable();
	//
	void EndRootRegistrationTable();
	//
	void StartRegistrationTable(const char *tableName);
	//
	void EndRegistrationTable();


	lua_State *L;
	lua_State * m_luaStates[LuaThread_Count];

	private:
		static Handle s_myHandle;
}; // class LuaEnvironment

}; // namespace Components
}; // namespace PE

#endif
