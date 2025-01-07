#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"


#include "ServerGame.h"

#include "PrimeEngine/RenderJob.h"

#if APIABSTRACTION_PS3
#include <cell/sysmodule.h>
#include <sys/process.h>
#include <sys/spu_initialize.h>
#include <sys/paths.h>
#endif

// Static member variables
PE::Components::ServerGame* PE::Components::ServerGlobalGameCallbacks::s_gamePointer;


PE::Components::ServerGame::EngineInitParams PE::Components::ServerGame::EngineInitParams::s_params;
PE::GameContext PE::Components::ServerGame::s_context;
PE::Components::ServerGlobalGameCallbacks::StaticGameConstruct PE::Components::ServerGlobalGameCallbacks::s_constructFunction = &PE::Components::ServerGame::ConstructCallback;
PE::Components::ServerGlobalGameCallbacks::StaticInitEngine PE::Components::ServerGlobalGameCallbacks::s_initEngineFunction = &PE::Components::ServerGame::InitEngineCallback;

namespace PE {
using namespace Events;
namespace Components {
	ServerGame::ServerGame(PE::GameContext &context, PE::MemoryArena arena, Handle hMyself)
: Component(context, arena, hMyself)
{
	m_pTimer = new(MemoryArena_Server) Timer();

	m_frameTime = 0;
	m_gameTime = 0;
}

ServerGame::~ServerGame()
{
	m_pTimer->~Timer();
	free(m_pTimer);
}

int ServerGame::initEngine(GameContext &context, PE::MemoryArena arena, EngineInitParams &engineParams)
{
	PEINFO("Server: ServerGame::initEngine()\n");

	#if PE_PLAT_IS_WIN32
		context.m_pMPArgs = new(arena) MainFunctionArgs(context, arena, engineParams.lpCmdLine, engineParams.hInstance);
	#else
		context.m_pMPArgs = new(arena) MainFunctionArgs(context, arena, engineParams.lpCmdLine);
	#endif

	context.m_pLuaEnv = new(arena) ServerLuaEnvironment(context, arena, Handle());
	context.getLuaEnvironment()->registerInitialLibrariesFunctions();
	context.getLuaEnvironment()->run();

	PEINFO("PROGRESS: LuaEnvironment Constructed and LuaEnvironmenr script has been run\n");

	PE::Register(context.getLuaEnvironment(), PE::GlobalRegistry::Instance());

	PEINFO("PROGRESS: Registered all PE Components and Events\n");

	{
		context.m_pLog = new(arena) Log(context, arena, Handle());
		context.getLog()->addDefaultComponents();
	}

	context.getLuaEnvironment()->addDefaultComponents(); // end lua component initialization
	PEINFO("PROGRESS: LuaEnvironment Constructed (Added event handlers now that the events are registered)\n");

	{
		context.m_pNetworkManager = new (arena) ServerNetworkManager(context, arena, Handle());
		context.getNetworkManager()->addDefaultComponents();
	}

	// register classes with network that could not be registered through constructor because network manager wasn created
	context.getLuaEnvironment()->m_networkId = Networkable::s_NetworkId_LuaEnvironment;
	context.getLuaEnvironment()->registerWithNetwork(context.getNetworkManager());


	{
		context.m_pGameObjectManager = new(arena) GameObjectManager(context, arena, Handle());
		context.getGameObjectManager()->addDefaultComponents();
	}

	PEINFO("PROGRESS: GameObjectManager Constructed\n");

	context.getGameObjectManager()->addComponent(context.getNetworkManager()->getHandle());

	context.getNetworkManager()->initNetwork();
	return 1;
}

int ServerGame::initGame()
{
	
    return 1;
}

int ServerGame::runGame()
{
    // Game Loop ---------------------------------------------------------------
    m_runGame = true;
    
	m_thread.m_function = &ServerGame::ServerThread;
	m_thread.m_pParams = this;
	
	m_thread.run();
	return 1;
}

void ServerGame::ServerThread(void *params)
{
	ServerGame *pServerGame = (ServerGame*)(params);
	while(pServerGame->m_runGame)
	{
		pServerGame->runGameFrame();
	} // while (runGame) -- game loop

	return;
}

int ServerGame::runGameFrame()
{
	//m_frameTime = m_pTimer->TickAndGetTimeDeltaInSeconds();
	
	m_gameTime += m_frameTime;

	Event_UPDATE updateEvent;
	updateEvent.m_frameTime = m_frameTime;

	m_pContext->getGameObjectManager()->handleEvent(&updateEvent);

	return 0;
}
}; // namespace Components
}; // namespace PE
