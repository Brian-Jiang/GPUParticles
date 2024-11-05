#ifndef __PYENGINE_2_0_CLIENT_LUA_COMPONENT_H__
#define __PYENGINE_2_0_CLIENT_LUA_COMPONENT_H__

// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

#include "PrimeEngine/Lua/LuaEnvironment.h"

namespace PE {
namespace Components {
struct ClientLuaEnvironment : public LuaEnvironment
{
	PE_DECLARE_CLASS(ClientLuaEnvironment);

	ClientLuaEnvironment(PE::GameContext &context, PE::MemoryArena arena, Handle hMyself);
	virtual void registerInitialLibrariesFunctions();
	virtual void run();

	virtual void do_UPDATE(Events::Event *pEvt);


	static int l_getGameContext(lua_State* luaVM);
	static int l_changeRenderMode(lua_State* luaVM);

	int m_framesSinceLastFailedInit;
	
}; // class ClientLuaEnvironment

}; // namespace Components
}; // namespace PE

#endif
