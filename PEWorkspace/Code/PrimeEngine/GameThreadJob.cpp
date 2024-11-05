#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"
#include "GameThreadJob.h"
#include "RenderJob.h"
#include "PrimeEngine/Physics/PhysicsManager.h"

#include "PrimeEngine/Scene/DrawList.h"

#if APIABSTRACTION_IOS
#import <QuartzCore/QuartzCore.h>
#import <UIKit/UIKit.h>
#import <CoreData/CoreData.h>
#endif

extern "C"{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
};

#include "PrimeEngine/Game/Client/ClientGame.h"
#include "Application/Application.h"

namespace PE {

using namespace Events;
using namespace Components;

void gameThreadFunctionJob(void *params)
{
	g_gameThreadInitializationLock.lock();
	//initialization here..
	g_gameThreadInitialized = true;
	g_gameThreadExited = false;
	g_gameThreadInitializationLock.unlock();

	// signal main thread we are done initializing this thread
	g_gameThreadInitializedCV.signal();
	while (1)
    {
		ClientGame::runGameFrameStatic();
    }

    return;
}

namespace Components{

// single frame code
int ClientGame::runGameFrame()
{
    // Frame Start ---------------------------------------------------------
    float gameTimeBetweenFrames = m_hTimer.getObject<Timer>()->TickAndGetTimeDeltaInSeconds(); // time spent in ios before resuming this thread
    float gameThreadPreDrawFrameTime = 0;
	float gameThreadDrawFrameTime = 0;
	float gameThreadDrawWaitFrameTime = 0;

	// cache root scene node pointer
    RootSceneNode *proot = RootSceneNode::Instance();
    
    //Start Logging events for this frame if requested
    if(m_pContext->getLog()->m_activateOnNextFrame)
    {
        m_pContext->getLog()->m_isActivated = true;
        m_pContext->getLog()->m_activateOnNextFrame = false;
    }
    
    // CLOSED_WINDOW event will be pushed into global event queue if user closes window after this call
    m_pContext->getApplication()->processOSEventsIntoGlobalEventQueue();
    
    //Create Physics Events
    {
        Handle hphysStartEvent("EVENT", sizeof(Event_PHYSICS_START));
        Event_PHYSICS_START *physStartEvent = new(hphysStartEvent) Event_PHYSICS_START ;
        
        physStartEvent->m_frameTime = m_frameTime;
        Events::EventQueueManager::Instance()->add(hphysStartEvent, Events::QT_GENERAL);

		//Handle hphysUpdateEvent("EVENT", sizeof(Event_Update_Physics));
		//Event_Update_Physics* physUpdateEvent = new(hphysUpdateEvent) Event_Update_Physics;

		//physUpdateEvent->m_frameTime = m_frameTime;
		//Events::EventQueueManager::Instance()->add(hphysUpdateEvent, Events::QT_GENERAL);
    }
    // Create UPDATE event
    {
        Handle hupdateEvent("EVENT", sizeof(Event_UPDATE));
        Event_UPDATE *updateEvent = new(hupdateEvent) Event_UPDATE ;
        
        updateEvent->m_frameTime = m_frameTime;
        
        //Push UPDATE event onto general queue
        Events::EventQueueManager::Instance()->add(hupdateEvent, Events::QT_GENERAL);
    }
    // Create SCENE_GRAPH_UPDATE event 
    {
        Handle hsgUpdateEvent("EVENT", sizeof(Event_SCENE_GRAPH_UPDATE));
        Event_SCENE_GRAPH_UPDATE *sgUpdateEvent = new(hsgUpdateEvent) Event_SCENE_GRAPH_UPDATE ;
        
        sgUpdateEvent->m_frameTime = m_frameTime;
        //Push event into general queue
        Events::EventQueueManager::Instance()->add(hsgUpdateEvent, Events::QT_GENERAL);
    }
    
    //Assign camera
    Handle hCam = CameraManager::Instance()->getActiveCameraHandle();
    CameraSceneNode *pcam = CameraManager::Instance()->getActiveCamera()->getCamSceneNode();
    
    // Push Event_CALCULATE_TRANSFORMATIONS
    {
        Handle hctevt("EVENT", sizeof(Event_CALCULATE_TRANSFORMATIONS));
        /*Event_CALCULATE_TRANSFORMATIONS *ctevt = */ new(hctevt) Event_CALCULATE_TRANSFORMATIONS ;
        
        Events::EventQueueManager::Instance()->add(hctevt, Events::QT_GENERAL);
    }

	{
		Handle hphysUpdateEvent("EVENT", sizeof(Event_Update_Physics));
		Event_Update_Physics* physUpdateEvent = new(hphysUpdateEvent) Event_Update_Physics;

		physUpdateEvent->m_frameTime = m_frameTime;
		Events::EventQueueManager::Instance()->add(hphysUpdateEvent, Events::QT_GENERAL);
	}
    
	g_physicsThreadLock.lock();
    // Process general events (Draw, Update, Calculate transformations...)
    Handle gqh = Events::EventQueueManager::Instance()->getEventQueueHandle("general");
    while (!gqh.getObject<Events::EventQueue>()->empty())
    {
        Events::Event *pGeneralEvt = gqh.getObject<Events::EventQueue>()->getFront();
        // this code is in process of conversion to new event style
        // first use new method then old (switch)
        if (Event_UPDATE::GetClassId() == pGeneralEvt->getClassId())
        {
            // UPDATE
            // Update game objects
            m_pContext->getGameObjectManager()->handleEvent(pGeneralEvt);
        }
        else if (Event_CALCULATE_TRANSFORMATIONS::GetClassId() == pGeneralEvt->getClassId())
        {
            
            // for all scene objects to calculate their absolute (world) transformations
            // for skins to calculate their matrix palettes
            proot->handleEvent(pGeneralEvt);
            
            //SkyVolume::Instance()->handleEvent(pGeneralEvt);
            
            // Generate Drawing Events

			// Push Event_PRE_GATHER_DRAWCALLS
			{
				Handle hctevt("EVENT", sizeof(Event_PRE_GATHER_DRAWCALLS));
				Event_PRE_GATHER_DRAWCALLS *ctevt =  new(hctevt) Event_PRE_GATHER_DRAWCALLS ;
        
				PE::Events::EventQueueManager::Instance()->add(hctevt, Events::QT_GENERAL);

				ctevt->m_projectionViewTransform = pcam->m_viewToProjectedTransform * pcam->m_worldToViewTransform;
				ctevt->m_eyePos = pcam->m_worldTransform.getPos();

				
			}

            {
                Handle hdrawZOnlyEvt("EVENT", sizeof(Event_GATHER_DRAWCALLS_Z_ONLY));
                Event_GATHER_DRAWCALLS_Z_ONLY *drawZOnlyEvt = new(hdrawZOnlyEvt) Event_GATHER_DRAWCALLS_Z_ONLY ;
                
                drawZOnlyEvt->m_pZOnlyDrawListOverride = 0;

				RootSceneNode *pRoot = RootSceneNode::Instance();

				if (pRoot->m_lights.m_size)
				{
					PrimitiveTypes::Bool foundShadower = false;
					for(PrimitiveTypes::UInt32 i=0; i<(pRoot->m_lights.m_size); i++){
						Light *pLight = pRoot->m_lights[i].getObject<Light>();
						if(pLight->castsShadow()){

							drawZOnlyEvt->m_projectionViewTransform = (pLight->m_viewToProjectedTransform * pLight->m_worldToViewTransform);
							drawZOnlyEvt->m_eyePos = pLight->m_base.getPos();
							foundShadower=true;
							break;
						}
						if(!foundShadower){
							drawZOnlyEvt->m_projectionViewTransform = pcam->m_viewToProjectedTransform * pcam->m_worldToViewTransform;
							drawZOnlyEvt->m_eyePos = pcam->m_worldTransform.getPos();
						}
					}
				}
				else
				{
					drawZOnlyEvt->m_projectionViewTransform = pcam->m_viewToProjectedTransform * pcam->m_worldToViewTransform;
					drawZOnlyEvt->m_eyePos = pcam->m_worldTransform.getPos();
				}
                drawZOnlyEvt->m_parentWorldTransform.loadIdentity();
                Events::EventQueueManager::Instance()->add(hdrawZOnlyEvt);
            }
            
            // After the transformations are done. We can put a DRAW event in the queue
            // Push DRAW event into message queue because camera has updated transformations
            {
                Handle hdrawEvt("EVENT", sizeof(Event_GATHER_DRAWCALLS));
                Event_GATHER_DRAWCALLS *drawEvt = new(hdrawEvt) Event_GATHER_DRAWCALLS(m_pContext->m_gameThreadThreadOwnershipMask) ;
                
                drawEvt->m_frameTime = m_frameTime;
				drawEvt->m_frameCount = m_frameCount;
                drawEvt->m_gameTime = m_gameTime;
                
                drawEvt->m_drawOrder = EffectDrawOrder::First;
                
                //Camera *pcam = hcam.getObject<Camera>();
                drawEvt->m_projectionViewTransform = pcam->m_viewToProjectedTransform * pcam->m_worldToViewTransform;

                drawEvt->m_eyePos = pcam->m_worldTransform.getPos();
				drawEvt->m_projectionTransform = pcam->m_viewToProjectedTransform;
				drawEvt->m_eyeDir = pcam->m_worldTransform.getN();
                drawEvt->m_parentWorldTransform.loadIdentity();
                drawEvt->m_viewInvTransform = pcam->m_worldToViewTransform.inverse();
				drawEvt->m_frustrum = pcam->m_frustrum;
                
				//Commented out by Mac because I'm pretty sure this does nothing but am afraid to delete it...
				static bool setCameraAsLightSource = false;
				RootSceneNode *pRoot = RootSceneNode::Instance();
				if (setCameraAsLightSource && pRoot->m_lights.m_size)
				{
					Light *pLight = pRoot->m_lights[0].getObject<Light>();

					drawEvt->m_projectionViewTransform = (pLight->m_viewToProjectedTransform * pLight->m_worldToViewTransform);
					drawEvt->m_eyePos = pLight->m_base.getPos();
				}
				
                Events::EventQueueManager::Instance()->add(hdrawEvt);
            }
            
            {
                Handle hphysicsEndEvt("EVENT", sizeof(Event_PHYSICS_END));
                /*Event_PHYSICS_END *physicsEndEvt = */ new(hphysicsEndEvt) Event_PHYSICS_END ;
                
                Events::EventQueueManager::Instance()->add(hphysicsEndEvt);
            }
            
        }

		else if (Event_PRE_GATHER_DRAWCALLS::GetClassId() == pGeneralEvt->getClassId())
        {
			proot->handleEvent(pGeneralEvt);
		}
        
        else if (Event_GATHER_DRAWCALLS::GetClassId() == pGeneralEvt->getClassId()
			|| Event_GATHER_DRAWCALLS_Z_ONLY::GetClassId() == pGeneralEvt->getClassId())
        {
			bool zOnly = Event_GATHER_DRAWCALLS_Z_ONLY::GetClassId() == pGeneralEvt->getClassId();
			
            Event_GATHER_DRAWCALLS *pDrawEvent = NULL;
			Event_GATHER_DRAWCALLS_Z_ONLY *pDrawZOnlyEvent = NULL;
			if (zOnly)
			{
				pDrawZOnlyEvent = (Event_GATHER_DRAWCALLS_Z_ONLY *)(pGeneralEvt);
				DrawList::ZOnlyInstance()->reset();

			}
			else
			{
				pDrawEvent = (Event_GATHER_DRAWCALLS *)(pGeneralEvt);
				DrawList::Instance()->reset();
			}

			if (pDrawEvent)
				EffectManager::Instance()->m_currentViewProjMatrix = pDrawEvent->m_projectionViewTransform;
          
			// Draw 1st order
			proot->handleEvent(pGeneralEvt);

			// for non z only we do several draw order passes
			if (pDrawEvent)
			{
				/*
				// Manual draw pass
				pDrawEvent->m_drawOrder = EffectDrawOrder::Manual;
			
				// Draw SkyVolume
				//SkyVolume::Instance()->handleEvent(pGeneralEvt);
				*/

				EffectManager::Instance()->m_doMotionBlur = true;

				// Draw Last order
				pDrawEvent->m_drawOrder = EffectDrawOrder::Last;
			
				proot->handleEvent(pGeneralEvt);
			}

			// this code will make sure draw thread know we are done
			// only do it for DRAW event since is last draw event
			// and in case we don't do multithreading, we just execute the draw thread function

			if (!zOnly)
			{
				gameThreadPreDrawFrameTime = m_hTimer.getObject<Timer>()->TickAndGetTimeDeltaInSeconds();

				static bool s_RenderOnGameThread = false; // if this is true, render thread will never wake up

				#if PYENGINE_2_0_MULTI_THREADED
					g_drawThreadLock.lock(); // wait till previous draw is finished
					//PEINFO("Game thread got g_drawThreadLock\n");
				#endif

				// this thread now can have control of rendering context for a little bit
				// we will pass down this variable to different functions to let them know that we have both contexts
				m_pContext->getGPUScreen()->AcquireRenderContextOwnership(m_pContext->m_gameThreadThreadOwnershipMask);
				 
				gameThreadDrawWaitFrameTime = m_hTimer.getObject<Timer>()->TickAndGetTimeDeltaInSeconds();
				
				#if PE_ENABLE_GPU_PROFILING
					// finalize results of gpu profiling. we want to do it in this thread to avoid race condition
					// since we are accessing debug renderer. 
					Profiling::Profiler::Instance()->finalize(Profiling::Group_DrawCalls);
					Profiling::Profiler::Instance()->finalize(Profiling::Group_DrawThread);
				#endif

                PE::IRenderer::checkForErrors("");

				// send this event to objects so that they have ability to work with graphics resources
				// since this thread has ownership of dx thread
				Event_PRE_RENDER_needsRC preRenderEvt(m_pContext->m_gameThreadThreadOwnershipMask);
				m_pContext->getGameObjectManager()->handleEvent(&preRenderEvt);
				proot->handleEvent(&preRenderEvt);
				
                PE::IRenderer::checkForErrors("");

				//FPS
				{
					float fps = (1.0f/m_frameTime);
					sprintf(PEString::s_buf, "%.2f FPS", fps);
					DebugRenderer::Instance()->createTextMesh(
						PEString::s_buf, true, false, false, false, 0, 
						Vector3(.75f, .05f, 0), 1.0f, m_pContext->m_gameThreadThreadOwnershipMask);
				}

                PE::IRenderer::checkForErrors("");

				//LUA Command Server, Server
				PE::GameContext *pServer = &PE::Components::ServerGame::s_context;
				{
					sprintf(PEString::s_buf, "Lua Command Receiver Ports: Client: %d Server: %d", m_pContext->getLuaCommandServerPort(), pServer->getLuaEnvironment() ? pServer->getLuaCommandServerPort():0);
					DebugRenderer::Instance()->createTextMesh(
						PEString::s_buf, true, false, false, false, 0,
						Vector3(.0f, .05f, 0), 1.0f, m_pContext->m_gameThreadThreadOwnershipMask);
				}

                PE::IRenderer::checkForErrors("");

				if (pServer->getLuaEnvironment()) // check if server context was initialized
				{
					ServerNetworkManager *pServerNetw = (ServerNetworkManager*)(pServer->getNetworkManager());
					pServerNetw->debugRender(m_pContext->m_gameThreadThreadOwnershipMask, 0, .1f);
				}
                
                PE::IRenderer::checkForErrors("");

				{
					ClientNetworkManager* pClientNetworkManager = (ClientNetworkManager* )(m_pContext->getNetworkManager());
					pClientNetworkManager->debugRender(m_pContext->m_gameThreadThreadOwnershipMask, 0.5f, .1f);
				}
                
                PE::IRenderer::checkForErrors("");

				//gameplay timer
				{
					sprintf(PEString::s_buf, "GT frame wait:%.3f pre-draw:%.3f+render wait:%.3f+render:%.3f+post-render:%.3f = %.3f sec\n", m_gameTimeBetweenFrames, m_gameThreadPreDrawFrameTime, m_gameThreadDrawWaitFrameTime, m_gameThreadDrawFrameTime, m_gameThreadPostDrawFrameTime, m_frameTime);
					DebugRenderer::Instance()->createTextMesh(
						PEString::s_buf, true, false, false, false, 0,
						Vector3(.0f, .075f, 0), 1.0f, m_pContext->m_gameThreadThreadOwnershipMask);
				}
				
				//debug draw root and grid
				DebugRenderer::Instance()->createRootLineMesh();// send event while the array is on the stack

				// call this to potentially generate meshes that were scheduled in debug draw of lines
				DebugRenderer::Instance()->postPreDraw(m_pContext->m_gameThreadThreadOwnershipMask);

                PE::IRenderer::checkForErrors("");

				// done synchronization, give RC back
				m_pContext->getGPUScreen()->ReleaseRenderContextOwnership(m_pContext->m_gameThreadThreadOwnershipMask);
				
				DrawList::swap();

				#if PYENGINE_2_0_MULTI_THREADED
					if (!s_RenderOnGameThread) // this variable will dynamically keep other thread waiting
						g_drawThreadCanStart = true;
					else
						runDrawThreadSingleFrame(*m_pContext); // tun on game thread
					//PEINFO("Game thread releasing g_drawThreadLock\n");
					g_drawThreadLock.unlock(); // release render thread lock
					g_drawCanStartCV.signal();
				#else
					runDrawThreadSingleFrame(*m_pContext);
				#endif

				gameThreadDrawFrameTime = m_hTimer.getObject<Timer>()->TickAndGetTimeDeltaInSeconds();
			}
        }
        else if (Event_FLY_CAMERA::GetClassId() == pGeneralEvt->getClassId())
        {
            Event_FLY_CAMERA *pRealEvent = (Event_FLY_CAMERA *)(pGeneralEvt);
            pcam->m_base.moveForward(pRealEvent->m_relativeMove.getZ());
            pcam->m_base.moveRight(pRealEvent->m_relativeMove.getX());
            pcam->m_base.moveUp(pRealEvent->m_relativeMove.getY());
        }
        else if (Event_ROTATE_CAMERA::GetClassId() == pGeneralEvt->getClassId())
        {
            Event_ROTATE_CAMERA *pRealEvent = (Event_ROTATE_CAMERA *)(pGeneralEvt);
            pcam->m_base.turnUp(pRealEvent->m_relativeRotate.getY());
            pcam->m_base.turnAboutAxis(-pRealEvent->m_relativeRotate.getX(), RootSceneNode::Instance()->m_worldTransform.getV());
        }
        else if (Event_CLOSED_WINDOW::GetClassId() == pGeneralEvt->getClassId())
        {
            m_runGame = false;
			break;
        }
        else if (Event_SCENE_GRAPH_UPDATE::GetClassId() == pGeneralEvt->getClassId())
        {
            // this event is meant for scene graph
            proot->handleEvent(pGeneralEvt);
        }
        else if (Event_PHYSICS_END::GetClassId() == pGeneralEvt->getClassId())
        {
			// fetch physics results
        }
        else if (Event_PHYSICS_START::GetClassId() == pGeneralEvt->getClassId())
        {
			// physics kick off
        }
		else if (Event_Update_Physics::GetClassId() == pGeneralEvt->getClassId()) {
			//PhysicsManager::Instance()->handleEvent(pGeneralEvt);
			g_physicsThreadCanStart = true;
			g_physicsThreadLock.unlock();
			g_physicsCanStartCV.signal();
		}
        else
        {
            // unknown event
            // pass it to both game object manager and scene graph
            
           m_pContext->getGameObjectManager()->handleEvent(pGeneralEvt);
            proot->handleEvent(pGeneralEvt);
            
        } // end of old event style handling
        
        gqh.getObject<Events::EventQueue>()->destroyFront();
    }
    
    // Events are destoryed by destroyFront() but this is called every frame just in case
    gqh.getObject<Events::EventQueue>()->destroy();
    
    // End of frame --------------------------------------------------------
    
    // add memory defragmentation here
    // after this all pointers must be recalculated from handles
    
    //Stop logging events for this frame if was requested to log in this frame
    m_pContext->getLog()->m_isActivated = false;
    
	float gameThreadPostDrawFrameTime = m_hTimer.getObject<Timer>()->TickAndGetTimeDeltaInSeconds();
    
	++m_frameCount;
	m_frameTime = gameTimeBetweenFrames + gameThreadPreDrawFrameTime + gameThreadDrawWaitFrameTime + gameThreadDrawFrameTime + gameThreadPostDrawFrameTime;
	
    if (m_frameTime > 10.0f)
        m_frameTime = 0.1f;
    m_gameTimeBetweenFrames = gameTimeBetweenFrames;
    m_gameThreadPreDrawFrameTime = gameThreadPreDrawFrameTime;
	m_gameThreadDrawWaitFrameTime = gameThreadDrawWaitFrameTime;
	m_gameThreadDrawFrameTime = gameThreadDrawFrameTime;
	m_gameThreadPostDrawFrameTime = gameThreadPostDrawFrameTime;

	m_gameTime += m_frameTime;

	if (!m_runGame)
	{
		PE::GameContext *pServer = &PE::Components::ServerGame::s_context;
		if (pServer->getGame())
		{
			((ServerGame *)(pServer->getGame()))->m_runGame = false;
		}

		// this is the last iteration of game thread, wait for all other threads to be done with current frame
		#if PYENGINE_2_0_MULTI_THREADED
			g_drawThreadLock.lock(); // wait till previous draw is finished
			g_drawThreadShouldExit = true;
			
			g_drawThreadLock.unlock(); // release render thread lock
			// wake up render thread
			g_drawCanStartCV.signal();
			
			while (!g_drawThreadExited)
			{
				//wait until the draw thread exits
			}
		#endif
	}

	m_pContext->getApplication()->exit(); // this will close the window
	return 0;

}
};
}; // namespace PE

