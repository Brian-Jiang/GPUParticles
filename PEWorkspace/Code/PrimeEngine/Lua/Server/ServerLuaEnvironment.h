#ifndef __PYENGINE_2_0_SERVER_LUA_COMPONENT_H__
#define __PYENGINE_2_0_SERVER_LUA_COMPONENT_H__

// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

// Outer-Engine includes
#include <assert.h>

#include "PrimeEngine/Lua/LuaEnvironment.h"

namespace PE {
namespace Components {
struct ServerLuaEnvironment : public LuaEnvironment
{
	PE_DECLARE_CLASS(ServerLuaEnvironment);

	ServerLuaEnvironment(PE::GameContext &context, PE::MemoryArena arena, Handle hMyself);
	virtual void registerInitialLibrariesFunctions();
	virtual void run();
	virtual void do_UPDATE(Events::Event *pEvt);

	static int l_getGameContext(lua_State* luaVM);

	int m_framesSinceLastFailedInit;
}; // class ServerLuaEnvironment

}; // namespace Components
}; // namespace PE

#endif
