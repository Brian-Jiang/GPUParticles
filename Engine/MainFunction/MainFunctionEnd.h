
#define RETURN_VALUE 0
#if PE_PLAT_IS_IOS
#undef RETURN_VALUE
#define RETURN_VALUE
#endif

		//params were set before this file's #include

		// some global initializations go here..
		memset(&PE::Components::ClientGame::s_context, 0, sizeof(PE::Components::ClientGame::s_context));
		memset(&PE::Components::ServerGame::s_context, 0, sizeof(PE::Components::ServerGame::s_context));

		PE::Components::ClientGame::s_context.m_defaultArena = PE::MemoryArena_Client;
		PE::Components::ServerGame::s_context.m_defaultArena = PE::MemoryArena_Server;

		PE::Components::ClientGame::s_context.m_luaCommandServerPort = PE_CLIENT_LUA_COMMAND_SERVER_PORT;
		PE::Components::ServerGame::s_context.m_luaCommandServerPort = PE_SERVER_LUA_COMMAND_SERVER_PORT;

		PE::Components::ClientGame::s_context.m_isServer = false;
		PE::Components::ServerGame::s_context.m_isServer = true;


		// Begin Client Initialization
		{
			//initialize engine by calling the initEngine callback
			if (!PE::Components::ClientGlobalGameCallbacks::InitEngine(PE::Components::ClientGame::s_context , PE::MemoryArena_Client)) return RETURN_VALUE;

			//Create game singleton. Has to be done after initializing PrimeEngine in order to use MemoryManager
			// this calls the PE::Components::ClientGlobalGameCallbacks::s_constructFunction callback to create correct game instance
			PE::Components::ClientGlobalGameCallbacks::Construct(PE::Components::ClientGame::s_context , PE::MemoryArena_Client);
		
			//Initialize game
			if(!((PE::Components::ClientGame*)(PE::Components::ClientGame::s_context.getGame()))->initGame())
				return RETURN_VALUE;

			PE::GlobalRegistry::Instance()->setInitialized(true); // since server was initializes, we must have registered classes already
		}
		// End Client Initialization

		// Begin Server Initialization
		if (true)
		{
			//initialize engine by calling the initEngine callback
			if (!PE::Components::ServerGlobalGameCallbacks::InitEngine(PE::Components::ServerGame::s_context, PE::MemoryArena_Server)) return RETURN_VALUE;

			//Create game singleton. Has to be done after initializing PrimeEngine in order to use MemoryManager
			// this calls the PE::Components::ServerGlobalGameCallbacks::s_constructFunction callback to create correct game instance
			PE::Components::ServerGlobalGameCallbacks::Construct(PE::Components::ServerGame::s_context , PE::MemoryArena_Server);

			//Initialize game
			if(!PE::Components::ServerGlobalGameCallbacks::getGameInstance()->initGame())
				return RETURN_VALUE;

		}
		// End Server Initialization


		//Run the game

		if(!PE::Components::ServerGlobalGameCallbacks::getGameInstance()->runGame()) 
			return RETURN_VALUE;

        if(!PE::Components::ClientGlobalGameCallbacks::getGameInstance()->runGame()) 
			return RETURN_VALUE;

		return RETURN_VALUE;
#if !PE_PLAT_IS_IOS
	}
#endif
