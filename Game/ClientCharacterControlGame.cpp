
#include "ClientCharacterControlGame.h"

#include "PrimeEngine/Scene/SkeletonInstance.h"

#include "ClientGameObjectManagerAddon.h"
#include "Tank/ClientTank.h"
#include "Client/ClientSpaceShipControls.h"
#include "Characters/SoldierNPCAnimationSM.h"
#include "CharacterControl/Characters/SoldierNPCAnimationSM.h"
#include "CharacterControlContext.h"
#include "Characters/EnemyNPCMovementSM.h"
#if PE_PLAT_IS_WIN32
#include "test.h"
#endif

using namespace PE;
using namespace PE::Components;

namespace CharacterControl {
namespace Components {

// is run after initializing the engine
// basic demo just adds a light source and a default control scheme
// so that uses can add simple objects like meshes, skins, light sources, levels, etc. from asset manager
int ClientCharacterControlGame::initGame()
{
	// super implementation
	ClientGame::initGame();

	//add game specific context
	CharacterControlContext *pGameCtx = new (m_arena) CharacterControlContext;

	m_pContext->m_pGameSpecificContext = pGameCtx;

	PE::Components::LuaEnvironment *pLuaEnv = m_pContext->getLuaEnvironment();
	
	// init events, components, and other classes of the project
	CharacterControl::Register(pLuaEnv, PE::GlobalRegistry::Instance());

	// grey-ish background
	m_pContext->getGPUScreen()->setClearColor(Vector4(0.1f, 0.1f, 0.1f, 0.0f));


	// game controls read input queue and post events onto general queue
	// the events from general queue are then passed on to game components
	PE::Handle hDefaultGameControls("GAME_CONTROLS", sizeof(DefaultGameControls));
	m_pContext->m_pDefaultGameControls = new(hDefaultGameControls) DefaultGameControls(*m_pContext, m_arena, hDefaultGameControls);
	m_pContext->m_pDefaultGameControls->addDefaultComponents();

	m_pContext->getGameObjectManager()->addComponent(hDefaultGameControls);

	// initialize game

	// create the GameObjectmanager addon that is in charge of game objects in this demo
	{
		// create the GameObjectmanager addon that is in charge of game objects in this demo
        PE::Handle hGOMAddon = PE::Handle("ClientGameObjectManagerAddon", sizeof(ClientGameObjectManagerAddon));
		pGameCtx->m_pGameObjectManagerAddon = new(hGOMAddon) ClientGameObjectManagerAddon(*m_pContext, m_arena, hGOMAddon);
		pGameCtx->getGameObjectManagerAddon()->addDefaultComponents();
	
		// add it to game object manager
		// now all game events will be passed through to our GameObjectManagerAddon
		m_pContext->getGameObjectManager()->addComponent(hGOMAddon);
	}

	bool spawnALotOfSoldiersForGpuAnim = false;

	//create tank controls that will be enabled if tank is activated
	{
		// create the GameObjectmanager addon that is in charge of game objects in this demo
		PE::Handle h("TankGameControls", sizeof(TankGameControls));
		pGameCtx->m_pTankGameControls = new(h) TankGameControls(*m_pContext, m_arena, h);
		pGameCtx->getTankGameControls()->addDefaultComponents();

		// add it to game object manager addon
		pGameCtx->getGameObjectManagerAddon()->addComponent(h);

		// start deactivated. needs to be deactivated AFTER adding it to parent components
		pGameCtx->getTankGameControls()->setEnabled(false);

		
#if !PE_API_IS_D3D11
		if (!spawnALotOfSoldiersForGpuAnim)
		{
			for (int i = 0; i < 6; ++i)
				((ClientGameObjectManagerAddon*)(pGameCtx->getGameObjectManagerAddon()))->createTank(
				i, m_pContext->m_gameThreadThreadOwnershipMask);
		}
#endif
	}

	{
		PE::Handle h("ClientSpaceShipControls", sizeof(SpaceShipGameControls));
		pGameCtx->m_pSpaceShipGameControls = new(h) SpaceShipGameControls(*m_pContext, m_arena, h);
		pGameCtx->getSpaceShipGameControls()->addDefaultComponents();

		// add it to game object manager addon
		pGameCtx->getGameObjectManagerAddon()->addComponent(h);

		// start deactivated. needs to be deactivated AFTER adding it to parent components
		pGameCtx->getSpaceShipGameControls()->setEnabled(false);

		if (false)
		{
			((ClientGameObjectManagerAddon*)(pGameCtx->getGameObjectManagerAddon()))->createSpaceShip(
				m_pContext->m_gameThreadThreadOwnershipMask);
		}
	}


	if (false)
	{
		PE::Handle hSN("SCENE_NODE", sizeof(SceneNode));
		SceneNode *pSN = new(hSN) SceneNode(*m_pContext, m_arena, hSN);
		pSN->addDefaultComponents();

		pSN->m_base.setPos(Vector3(0, 0, 0));

		{
			PE::Handle hSoldierAnimSM("SoldierNPCAnimationSM", sizeof(SoldierNPCAnimationSM));
			SoldierNPCAnimationSM *pSoldierAnimSM = new(hSoldierAnimSM) SoldierNPCAnimationSM(*m_pContext, m_arena, hSoldierAnimSM);
			pSoldierAnimSM->addDefaultComponents();

			pSoldierAnimSM->m_debugAnimIdOffset = 0;// rand() % 3;


			PE::Handle hSkeletonInstance("SkeletonInstance", sizeof(SkeletonInstance));
			SkeletonInstance *pSkelInst = new(hSkeletonInstance) SkeletonInstance(*m_pContext, m_arena, hSkeletonInstance, 
				hSoldierAnimSM);
			pSkelInst->addDefaultComponents();

			pSkelInst->initFromFiles("soldier_Soldier_Skeleton.skela", "Default", m_pContext->m_gameThreadThreadOwnershipMask);
			pSkelInst->setAnimSet("soldier_Soldier_Skeleton.animseta", "Default");

					
			{
				PE::Handle hMeshInstance("MeshInstance", sizeof(MeshInstance));
				MeshInstance *pMeshInstance = new(hMeshInstance) MeshInstance(*m_pContext, m_arena, hMeshInstance);
				pMeshInstance->addDefaultComponents();
				
				pMeshInstance->initFromFile("SoldierTransform.mesha", "Default", m_pContext->m_gameThreadThreadOwnershipMask);
				
				pSkelInst->addComponent(hMeshInstance);
			}

			
// 			{
// 				// create a scene node for gun attached to a joint
// 				PE::Handle hMyGunSN = PE::Handle("SCENE_NODE", sizeof(JointSceneNode));
// 				JointSceneNode *pGunSN = new(hMyGunSN) JointSceneNode(*m_pContext, m_arena, hMyGunSN, 38);
// 				pGunSN->addDefaultComponents();
// 				{
// 					PE::Handle hMyGunMesh = PE::Handle("MeshInstance", sizeof(MeshInstance));
// 					MeshInstance *pGunMeshInstance = new(hMyGunMesh) MeshInstance(*m_pContext, m_arena, hMyGunMesh);
// 
// 					pGunMeshInstance->addDefaultComponents();
// 					pGunMeshInstance->initFromFile(pEvt->m_gunMeshName, pEvt->m_gunMeshPackage, pEvt->m_threadOwnershipMask);
// 
// 					// add gun to joint
// 					pGunSN->addComponent(hMyGunMesh);
// 				}
// 				// add gun scene node to the skin
// 				pSkelInst->addComponent(hMyGunSN);
// 			}

		
			Events::SoldierNPCAnimSM_Event_WALK evt;
			pSkelInst->handleEvent(&evt);

			// add skeleton to scene node
			pSN->addComponent(hSkeletonInstance);
		}
		
		RootSceneNode::Instance()->addComponent(hSN);
	}

	//the soldier creation code expects not having the redner context so release it here, and reacquire afterwards
	m_pContext->getGPUScreen()->ReleaseRenderContextOwnership(m_pContext->m_gameThreadThreadOwnershipMask);
	
	#if PE_API_IS_D3D11
	if (spawnALotOfSoldiersForGpuAnim)
	{
		int smallx = 4;

		for (int y = 0; y < 16; ++y)
			for (int x = 0; x < smallx; ++x)
				((ClientGameObjectManagerAddon*)(m_pContext->get<CharacterControlContext>()->getGameObjectManagerAddon()))->createSoldierNPC(
				Vector3(x * 2.0f , 0.0f, 2.0f * y), m_pContext->m_gameThreadThreadOwnershipMask);
	}
	#endif

	{
		((ClientGameObjectManagerAddon*)(m_pContext->get<CharacterControlContext>()->getGameObjectManagerAddon()))->createTargetNPC(
			Vector3(0.f, 0.0f, 0.f), m_pContext->m_gameThreadThreadOwnershipMask);


		//PE::Handle hSN("SCENE_NODE", sizeof(SceneNode));
		//SceneNode* pMainSN = new(hSN) SceneNode(*m_pContext, m_arena, hSN);
		//pMainSN->addDefaultComponents();

		//pMainSN->m_base.setPos(Vector3(0.0f, 0, 0.0f));

		//PE::Handle hImrodMeshInst = PE::Handle("MeshInstance", sizeof(MeshInstance));
		//MeshInstance* pImrodMeshInst = new(hImrodMeshInst) MeshInstance(*m_pContext, m_arena, hImrodMeshInst);

		//pImrodMeshInst->addDefaultComponents();
		//pImrodMeshInst->initFromFile("imrod.x_imrodmesh_mesh.mesha", "Default", m_pContext->m_gameThreadThreadOwnershipMask);

		//pMainSN->addComponent(hImrodMeshInst);

  //      PE::Handle hMove = PE::Handle("MOVE", sizeof(EnemyNPCMovementSM));
  //      EnemyNPCMovementSM* pMove = new(hMove) EnemyNPCMovementSM(*m_pContext, m_arena, hMove);
  //      pMove->addDefaultComponents();
		//pMainSN->addComponent(hMove);

		//RootSceneNode::Instance()->addComponent(hSN);
	}
    
    m_pContext->getGPUScreen()->AcquireRenderContextOwnership(m_pContext->m_gameThreadThreadOwnershipMask);
	
    bool spawnALotOfMeshes = false;
    
    int maxX = 10; // maybe need more to get framerate lower
    
    if (spawnALotOfMeshes)
    {
        for (int ix = 0; ix < maxX; ++ix)
        {
            for (int iy = 0; iy < 10; ++iy)
            {
                PE::Handle hSN("SCENE_NODE", sizeof(SceneNode));
                SceneNode *pMainSN = new(hSN) SceneNode(*m_pContext, m_arena, hSN);
                pMainSN->addDefaultComponents();
        
                pMainSN->m_base.setPos(Vector3(ix * 2.0f, 0, -10.0f -iy * 2.0f));
        
                PE::Handle hImrodMeshInst = PE::Handle("MeshInstance", sizeof(MeshInstance));
                MeshInstance *pImrodMeshInst = new(hImrodMeshInst) MeshInstance(*m_pContext, m_arena, hImrodMeshInst);
        
                pImrodMeshInst->addDefaultComponents();
				pImrodMeshInst->initFromFile("imrod.x_imrodmesh_mesh.mesha", "Default", m_pContext->    m_gameThreadThreadOwnershipMask);
				pImrodMeshInst->setDrawBoundingBox(pMainSN, pMainSN);
                pMainSN->addComponent(hImrodMeshInst);
        
                RootSceneNode::Instance()->addComponent(hSN);
            }
        }
    }

	
#if PE_PLAT_IS_WIN32
	
	int testVar = number_cpp_extern + number_c_extern;// + TestExternIntVar;

	__asm {
		mov eax, [number_cpp_extern]
		add eax, [number_c_extern]
		mov [testVar], eax
		mov eax, [number_c_extern]
	}

	testfunc_c_cdecl();

	testfunc_c_stdcall();

	testfunc_c_fastcall();
#endif

	//here's how you would run this level trough code

	//whenever Lua Code is executed it assumes that the thread DOES NOT HAVE render context
	//so in this case we need to release render context (this function has render context)
	//and then reacquire once lua is done

	m_pContext->getGPUScreen()->ReleaseRenderContextOwnership(m_pContext->m_gameThreadThreadOwnershipMask);

#if PE_PLAT_IS_PSVITA // do it for ps3 becasue right now communication between pyClient and ps3 is not working
	//m_pContext->getLuaEnvironment()->runString("LevelLoader.loadLevel('ccontrollvl0.x_level.levela', 'CharacterControl')");
#endif
	//m_pContext->getLuaEnvironment()->runString("LevelLoader.loadLevel('char_highlight.x_level.levela', 'Basic')");

	m_pContext->getGPUScreen()->AcquireRenderContextOwnership(m_pContext->m_gameThreadThreadOwnershipMask);

	return 1; // 1 (true) = success. no errors. TODO: add error checking
}


}
}
