#ifndef _SERVER_CHARACTER_CONTROL_GAME_H_
#define _SERVER_CHARACTER_CONTROL_GAME_H_

#include "PrimeEngine/PrimeEngineIncludes.h"
#include "GlobalRegistry.h"

namespace CharacterControl {
namespace Components {

	struct ServerCharacterControlGame : public PE::Components::ServerGame
	{
		// virtual methods to be overridden by inherited games
		virtual void registerClasses() {}

		// Singleton ------------------------------------------------------------------
		static ServerGame* ConstructCallback(PE::GameContext &context, PE::MemoryArena arena)
		{
			ServerCharacterControlGame *pGame = new(arena) ServerCharacterControlGame(context, arena, PE::Handle());
			context.m_pGame = pGame;
			return pGame;
		}

		ServerCharacterControlGame(PE::GameContext &context, PE::MemoryArena arena, PE::Handle hMyself) : PE::Components::ServerGame(context, arena, hMyself)
		{
		}

		// Virtual methods
		virtual int initGame();
		//
		// override in case need special game loop. usually not needed
		//virtual int runGame();
	};

}; // namespace Components
}; // namespace Basic

#endif
