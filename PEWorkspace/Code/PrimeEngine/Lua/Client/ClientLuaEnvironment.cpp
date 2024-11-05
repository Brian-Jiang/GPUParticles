#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

#include "ClientLuaEnvironment.h"

#include "PrimeEngine/Game/Client/ClientGame.h"

namespace PE {
namespace Components {
PE_IMPLEMENT_CLASS1(ClientLuaEnvironment, LuaEnvironment);

ClientLuaEnvironment::ClientLuaEnvironment(PE::GameContext &context, PE::MemoryArena arena, Handle hMyself)
: LuaEnvironment(context, arena, hMyself)
, m_framesSinceLastFailedInit(-1)
{
}
		
void ClientLuaEnvironment::registerInitialLibrariesFunctions()
{
	LuaEnvironment::registerInitialLibrariesFunctions();

	lua_register(L, "l_getGameContext", l_getGameContext);
	lua_register(L, "l_changeRenderMode", l_changeRenderMode);
}

void ClientLuaEnvironment::run()
{
	runScriptDefaultPath("LuaEnvironment.lua");
	if (m_pContext->m_luaCommandServerPort == 0)
		m_framesSinceLastFailedInit = 0;
}

void ClientLuaEnvironment::do_UPDATE(Events::Event *pEvt)
{
	if (m_framesSinceLastFailedInit >= 0 && ++m_framesSinceLastFailedInit > 1000)
	{
		run(); // try to initialize again
	}

	return LuaEnvironment::do_UPDATE(pEvt);
}

int ClientLuaEnvironment::l_getGameContext(lua_State* luaVM)
{
	lua_pushlightuserdata(luaVM, &PE::Components::ClientGame::s_context);
	return 1;
}
int ClientLuaEnvironment::l_changeRenderMode(lua_State* luaVM)
{
	GameContext *pContext = (GameContext *)(lua_touserdata(luaVM, -1));
	lua_pop(luaVM, 1);

	int mode = (int)(pContext->getGPUScreen()->m_renderMode);
	mode = mode + 1;
	if (mode == IRenderer::RenderMode_Count)
		mode = 0;
	pContext->getGPUScreen()->m_renderMode = (IRenderer::RenderMode)(mode);
	return 0;
}
}; // namespace Components
}; // namespace PE


