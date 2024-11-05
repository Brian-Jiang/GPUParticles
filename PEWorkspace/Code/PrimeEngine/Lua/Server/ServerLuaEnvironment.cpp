#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

#include "ServerLuaEnvironment.h"

#include "PrimeEngine/Game/Server/ServerGame.h"
namespace PE {
namespace Components {

PE_IMPLEMENT_CLASS1(ServerLuaEnvironment, LuaEnvironment);
ServerLuaEnvironment::ServerLuaEnvironment(PE::GameContext &context, PE::MemoryArena arena, Handle hMyself)
: LuaEnvironment(context, arena, hMyself)
, m_framesSinceLastFailedInit(-1)
{
	
}

void ServerLuaEnvironment::run()
{
	runScriptDefaultPath("ServerLuaEnvironment.lua");
	if (m_pContext->m_luaCommandServerPort == 0)
		m_framesSinceLastFailedInit = 0; // start counter to try again later
}

int ServerLuaEnvironment::l_getGameContext(lua_State* luaVM)
{
	lua_pushlightuserdata(luaVM, &PE::Components::ServerGame::s_context);
	return 1;	
}

void ServerLuaEnvironment::registerInitialLibrariesFunctions()
{
	LuaEnvironment::registerInitialLibrariesFunctions();

	lua_register(L, "l_getGameContext", l_getGameContext);
}

void ServerLuaEnvironment::do_UPDATE(Events::Event *pEvt)
{
	if (m_framesSinceLastFailedInit >= 0 && ++m_framesSinceLastFailedInit > 1000)
	{
		run(); // try reinitializing again
	}

	return LuaEnvironment::do_UPDATE(pEvt);
}

}; // namespace Components
}; // namespace PE


