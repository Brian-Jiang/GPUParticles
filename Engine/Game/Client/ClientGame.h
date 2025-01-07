#ifndef __CLIENT_GAME_H__
#define __CLIENT_GAME_H__

#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

#define PYENGINE_2_0_MULTI_THREADED !APIABSTRACTION_IOS && !APIABSTRACTION_PS3 && !PE_PLAT_IS_PSVITA

#include "PrimeEngine/Render/IRenderer.h"
#include "PrimeEngine/Game/Common/GameContext.h"
#include "PrimeEngine/Events/Component.h"
#include "PrimeEngine/Math/Vector3.h"
#include "PrimeEngine/APIAbstraction/Timer/Timer.h"

extern int TestExternIntVar;

#if PE_PLAT_IS_IOS
@class IOSGameRenderViewController;
#endif

namespace PE {
	extern volatile bool g_drawThreadInitialized;
	extern volatile bool g_drawThreadCanStart;
	extern volatile bool g_drawThreadShouldExit;
	extern volatile bool g_drawThreadExited;

	extern volatile bool g_gameThreadInitialized;
	extern volatile bool g_gameThreadCanStart;
	extern volatile bool g_gameThreadShouldExit;
	extern volatile bool g_gameThreadExited;

    extern volatile bool g_physicsThreadInitialized;
    extern volatile bool g_physicsThreadCanStart;
    extern volatile bool g_physicsThreadShouldExit;
    extern volatile bool g_physicsThreadExited;

	extern Threading::Mutex g_drawThreadInitializationLock;
	extern Threading::ConditionVariable g_drawThreadInitializedCV;
	
	extern Threading::Mutex g_gameThreadInitializationLock;
	extern Threading::ConditionVariable g_gameThreadInitializedCV;
	
	extern Threading::Mutex g_drawThreadLock;
	extern Threading::ConditionVariable g_drawCanStartCV;

    extern Threading::Mutex g_physicsThreadInitializationLock;
    extern Threading::ConditionVariable g_physicsThreadInitializedCV;

    extern Threading::Mutex g_physicsThreadLock;
    extern Threading::ConditionVariable g_physicsCanStartCV;
	
	extern Threading::PEThread g_drawThread, g_gameThread, g_physicsThread;

    namespace Components {
        struct ClientGame;

        struct ClientGlobalGameCallbacks
        {
            typedef Handle (*StaticGameConstruct)(PE::GameContext &context, PE::MemoryArena arena);
			typedef int (*StaticInitEngine)(PE::GameContext &context, PE::MemoryArena arena);
			static StaticGameConstruct s_constructFunction;

			static StaticInitEngine s_initEngineFunction;
            static Handle s_gameHandle;
            
            static ClientGame *getGameInstance() { return s_gameHandle.getObject<ClientGame>(); }
            
            static Handle InstanceHandle() { return s_gameHandle; }
            
            static void SetGameInstanceHandle(const Handle &handle)
            {
                // Singleton
                s_gameHandle = handle;
            }
            
            static void Construct(PE::GameContext &context, PE::MemoryArena arena)
            {
                if (s_constructFunction)
                {
                    s_gameHandle = s_constructFunction(context, arena);
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
        
        
        struct ClientGame : public Component
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
            
                        // Singleton ------------------------------------------------------------------
            static Handle ConstructCallback(PE::GameContext &context, PE::MemoryArena arena)
            {
                Handle handle("GAME", sizeof(ClientGame));
                context.m_pGame = new(handle) ClientGame(context, arena, handle);
                return handle;
            }

			static int InitEngineCallback(PE::GameContext &context,  PE::MemoryArena arena)
			{
				return PE::Components::ClientGame::initEngine(context, arena, PE::Components::ClientGame::EngineInitParams::s_params);
			}
            
            ClientGame(PE::GameContext &context, PE::MemoryArena arena, Handle hMyself)
            : Component(context, arena, hMyself)
            #if APIABSTRACTION_IOS
                , m_pIOSGameRenderViewController(NULL)
            #endif
            {
                suspended = true;
                runPhysXSim = true;
                m_hTimer = Handle("TIMER", sizeof(Timer));
                
                m_frameTime = 0.033f;
                m_gameTime = 0;
            }

			static int initEngine(GameContext &context, PE::MemoryArena arena, EngineInitParams &engineParams);

            virtual int initGame();
            virtual int runGame();
            virtual int runGameFrame();
            static void runGameFrameStatic();
            static void dummyIdleFunction();
            
            #if APIABSTRACTION_IOS
                void setIOSGameRenderViewController(IOSGameRenderViewController *p){m_pIOSGameRenderViewController = p;}
            #endif
            
            bool suspended;
            bool runPhysXSim;
            Handle m_hTimer;
            
            Handle hDefaultGameControls;
            
            PrimitiveTypes::Float32 m_frameTime;
            PrimitiveTypes::UInt32 m_frameCount;
            PrimitiveTypes::Float32 m_gameTimeBetweenFrames;
			PrimitiveTypes::Float32 m_gameThreadPreDrawFrameTime;
			PrimitiveTypes::Float32 m_gameThreadDrawWaitFrameTime;
			PrimitiveTypes::Float32 m_gameThreadDrawFrameTime;
			PrimitiveTypes::Float32 m_gameThreadPostDrawFrameTime;

            PrimitiveTypes::Float32 m_gameTime;
            PrimitiveTypes::Bool m_runGame;
            
            
            #if APIABSTRACTION_IOS
                IOSGameRenderViewController *m_pIOSGameRenderViewController;
            #endif
            
			static GameContext s_context;

        };
                
    }; // namespace Components
}; // namespace PE

#endif