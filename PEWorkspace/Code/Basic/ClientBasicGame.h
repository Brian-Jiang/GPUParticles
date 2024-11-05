#ifndef _BASIC_GAME_H_
#define _BASIC_GAME_H_

#include "PrimeEngine/PrimeEngineIncludes.h"
#include "GlobalRegistry.h"

namespace Basic {
namespace Components {
    struct TankController : public PE::Components::Component
    {
        // component API
        PE_DECLARE_CLASS(TankController);
        
        TankController(PE::GameContext &context, PE::MemoryArena arena, PE::Handle myHandle, float speed); // cosntructor
        
        virtual void addDefaultComponents(); // adds default children and event handlers
        
        
        PE_DECLARE_IMPLEMENT_EVENT_HANDLER_WRAPPER(do_UPDATE);
        virtual void do_UPDATE(PE::Events::Event *pEvt);
        
        float m_timeSpeed;
        float m_time;
        Vector2 m_center;
        PrimitiveTypes::UInt32 m_counter;
    };
    
	struct ClientBasicGame : public PE::Components::ClientGame
	{
		static PE::Handle ConstructCallback(PE::GameContext &context, PE::MemoryArena arena)
		{
            PE::Handle handle("GAME", sizeof(ClientBasicGame));
			context.m_pGame = new(handle) ClientBasicGame(context, arena, handle);

			return handle;
		}

		ClientBasicGame(PE::GameContext &context, PE::MemoryArena arena, PE::Handle hMyself) : PE::Components::ClientGame(context, arena, hMyself)
		{
		}

		// Virtual methods
		// override initialization because we want to initialize a light source and a defautl control scheme for Basic Demo
		virtual int initGame();
		//
		// override in case need special game loop. usually not needed
		//virtual int runGame();

	private:
		
	};

}; // namespace Components
}; // namespace Basic

#endif
