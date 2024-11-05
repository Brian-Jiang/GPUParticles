#ifndef _CHARACTER_CONTROL_GAME_H_
#define _CHARACTER_CONTROL_GAME_H_

#include "PrimeEngine/PrimeEngineIncludes.h"
#include "GlobalRegistry.h"

namespace CharacterControl {
	namespace Components {

		struct ClientCharacterControlGame : public PE::Components::ClientGame
		{
			// virtual methods to be overridden by inherited games
			virtual void registerClasses() {}

			// Singleton ------------------------------------------------------------------
			static PE::Handle ConstructCallback(PE::GameContext &context, PE::MemoryArena arena)
			{
				PE::Handle handle("GAME", sizeof(ClientCharacterControlGame));
				context.m_pGame = new(handle) ClientCharacterControlGame(context, arena, handle);
				return handle;
			}

			ClientCharacterControlGame(PE::GameContext &context, PE::MemoryArena arena, PE::Handle hMyself) : PE::Components::ClientGame(context, arena, hMyself)
			{
			}

			// Virtual methods
			// override initialization because we want to initialize a light source and a defautl control scheme for Basic Demo
			virtual int initGame();
			//
			// override in case need special game loop. usually not needed
			//virtual int runGame();
		};

	}; // namespace Components
}; // namespace Basic

#endif

