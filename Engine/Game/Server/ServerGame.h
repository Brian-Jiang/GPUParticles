
#ifndef __PrimeEngine_ServerGame_h__
#define __PrimeEngine_ServerGame_h__

#include "PrimeEngine/Events/Component.h"
#include "PrimeEngine/Math/Vector3.h"
#include "PrimeEngine/APIAbstraction/Timer/Timer.h"
namespace PE {

    namespace Components {
        struct ServerGame;

        struct ServerGlobalGameCallbacks
        {
            typedef ServerGame* (*StaticGameConstruct)(GameContext &context, PE::MemoryArena arena);
			typedef int (*StaticInitEngine)(GameContext &context, PE::MemoryArena arena);
			static StaticGameConstruct s_constructFunction;

			static StaticInitEngine s_initEngineFunction;
            static ServerGame* s_gamePointer;
            
            static ServerGame *getGameInstance() { return s_gamePointer; }
            
            static void SetGameInstancePointer(ServerGame *p)
            {
                // Singleton
                s_gamePointer = p;
            }
            
            static void Construct(GameContext &context, PE::MemoryArena arena)
            {
                if (s_constructFunction)
                {
                    s_gamePointer = s_constructFunction(context, arena);
                }
            }
			static int InitEngine(GameContext &context, PE::MemoryArena arena)
			{
				if (s_initEngineFunction)
				{
					return s_initEngineFunction(context, arena);
				}
				return 0;
			}
        };
        
        
        struct ServerGame : public Component
        {
			struct EngineInitParams
			{
				static const int MAX_ARGS = 32;
				Vector2i m_windowRes;
				const char * m_windowCaption;

				#if APIABSTRACTION_D3D9 || APIABSTRACTION_D3D11 || APIABSTRACTION_GLPC
					HINSTANCE hInstance, hPrevInstance;
				#endif
				const char *lpCmdLine;
				int showCmd;

				const char * args[MAX_ARGS];
				int argc;

				EngineInitParams()
					: m_windowRes(320, 240)
					, m_windowCaption("")
					, lpCmdLine("")
					, showCmd(0)
					#if APIABSTRACTION_D3D9 || APIABSTRACTION_D3D11
						, hInstance(0) , hPrevInstance(0)
					#endif
				{}
				static EngineInitParams s_params;
            };
            
            static ServerGame *ConstructCallback(GameContext &context, PE::MemoryArena arena)
            {
				ServerGame *pGame = new(arena) ServerGame(context, arena, Handle());
				context.m_pGame = pGame;
                return pGame;
            }

			static int InitEngineCallback(GameContext &context, PE::MemoryArena arena)
			{
				return PE::Components::ServerGame::initEngine(context, arena, PE::Components::ServerGame::EngineInitParams::s_params);
			}
            
            ServerGame(PE::GameContext &context, PE::MemoryArena arena, Handle hMyself);
			~ServerGame();

			static int initEngine(GameContext &context, PE::MemoryArena arena, EngineInitParams &engineParams);
			static void ServerThread(void *params);

            virtual int initGame();
            virtual int runGame();
            virtual int runGameFrame();
            
            void createHandleTypeMap();
            
            bool suspended;
            bool runPhysXSim;
            Timer *m_pTimer;
            
            Handle hDefaultGameControls;
            
            PrimitiveTypes::Float32 m_frameTime;
			PrimitiveTypes::Float32 m_gameThreadPreDrawFrameTime;
			PrimitiveTypes::Float32 m_gameThreadDrawWaitFrameTime;
			PrimitiveTypes::Float32 m_gameThreadDrawFrameTime;
			PrimitiveTypes::Float32 m_gameThreadPostDrawFrameTime;

            PrimitiveTypes::Float32 m_gameTime;
            PrimitiveTypes::Bool m_runGame;

			Threading::PEThread m_thread;
			static GameContext s_context;
        };
                
    }; // namespace Components
}; // namespace PE

#endif