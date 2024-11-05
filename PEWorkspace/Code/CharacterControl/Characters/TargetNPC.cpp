#include "TargetNPC.h"
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

#include "PrimeEngine/Lua/LuaEnvironment.h"
#include "PrimeEngine/Scene/SkeletonInstance.h"
#include "PrimeEngine/Scene/MeshInstance.h"
#include "PrimeEngine/Scene/RootSceneNode.h"
#include "PrimeEngine/APIAbstraction/GPUBuffers/ParticleSetBufferGPU.h"
#include "EnemyNPCMovementSM.h"

using namespace PE;
using namespace PE::Components;
using namespace CharacterControl::Events;

namespace CharacterControl {
    namespace Components {
        PE_IMPLEMENT_CLASS1(TargetNPC, Component);

        TargetNPC::TargetNPC(PE::GameContext& context, PE::MemoryArena arena, PE::Handle hMyself, Events::Event_CreateTargetNPC* pEvt) 
            : Component(context, arena, hMyself)
        {
            // need to acquire redner context for this code to execute thread-safe
            m_pContext->getGPUScreen()->AcquireRenderContextOwnership(pEvt->m_threadOwnershipMask);

            PE::Handle hSN("SCENE_NODE", sizeof(SceneNode));
            SceneNode* pMainSN = new(hSN) SceneNode(*m_pContext, m_arena, hSN);
            pMainSN->addDefaultComponents();

            pMainSN->m_base.setPos(pEvt->m_pos);
            pMainSN->m_base.setU(pEvt->m_u);
            pMainSN->m_base.setV(pEvt->m_v);
            pMainSN->m_base.setN(pEvt->m_n);

			RootSceneNode::Instance()->addComponent(hSN);

			// add the scene node as component of soldier without any handlers. this is just data driven way to locate scnenode for soldier's components
			{
				static int allowedEvts[] = { 0 };
				addComponent(hSN, &allowedEvts[0]);
			}

//			int numskins = 1; // 8
//			for (int iSkin = 0; iSkin < numskins; ++iSkin) {
//				float z = (iSkin / 4) * 1.5f;
//				float x = (iSkin % 4) * 1.5f;
//				PE::Handle hSN("SCENE_NODE", sizeof(SceneNode));
//				SceneNode* pSN = new(hSN) SceneNode(*m_pContext, m_arena, hSN);
//				pSN->addDefaultComponents();
//
//				pSN->m_base.setPos(Vector3(x, 0, z));
//
//				// rotation scene node to rotate soldier properly, since soldier from Maya is facing wrong direction
//				PE::Handle hRotateSN("SCENE_NODE", sizeof(SceneNode));
//				SceneNode* pRotateSN = new(hRotateSN) SceneNode(*m_pContext, m_arena, hRotateSN);
//				pRotateSN->addDefaultComponents();
//
//				pSN->addComponent(hRotateSN);
//
//				pRotateSN->m_base.turnLeft(3.1415);
//
//				PE::Handle hSoldierAnimSM("SoldierNPCAnimationSM", sizeof(SoldierNPCAnimationSM));
//				SoldierNPCAnimationSM* pSoldierAnimSM = new(hSoldierAnimSM) SoldierNPCAnimationSM(*m_pContext, m_arena, hSoldierAnimSM);
//				pSoldierAnimSM->addDefaultComponents();
//
//				pSoldierAnimSM->m_debugAnimIdOffset = 0;// rand() % 3;
//
//				PE::Handle hSkeletonInstance("SkeletonInstance", sizeof(SkeletonInstance));
//				SkeletonInstance* pSkelInst = new(hSkeletonInstance) SkeletonInstance(*m_pContext, m_arena, hSkeletonInstance,
//					hSoldierAnimSM);
//				pSkelInst->addDefaultComponents();
//
//				pSkelInst->initFromFiles("soldier_Soldier_Skeleton.skela", "Soldier", pEvt->m_threadOwnershipMask);
//
//				pSkelInst->setAnimSet("soldier_Soldier_Skeleton.animseta", "Soldier");
//
//				PE::Handle hMeshInstance("MeshInstance", sizeof(MeshInstance));
//				MeshInstance* pMeshInstance = new(hMeshInstance) MeshInstance(*m_pContext, m_arena, hMeshInstance);
//				pMeshInstance->addDefaultComponents();
//
//				pMeshInstance->initFromFile(pEvt->m_meshFilename, pEvt->m_package, pEvt->m_threadOwnershipMask);
//
//				pSkelInst->addComponent(hMeshInstance);
//
//				// add skin to scene node
//				pRotateSN->addComponent(hSkeletonInstance);
//
//#if !APIABSTRACTION_D3D11
//				{
//					PE::Handle hMyGunMesh = PE::Handle("MeshInstance", sizeof(MeshInstance));
//					MeshInstance* pGunMeshInstance = new(hMyGunMesh) MeshInstance(*m_pContext, m_arena, hMyGunMesh);
//
//					pGunMeshInstance->addDefaultComponents();
//					pGunMeshInstance->initFromFile(pEvt->m_gunMeshName, pEvt->m_gunMeshPackage, pEvt->m_threadOwnershipMask);
//
//					// create a scene node for gun attached to a joint
//
//					PE::Handle hMyGunSN = PE::Handle("SCENE_NODE", sizeof(JointSceneNode));
//					JointSceneNode* pGunSN = new(hMyGunSN) JointSceneNode(*m_pContext, m_arena, hMyGunSN, 38);
//					pGunSN->addDefaultComponents();
//
//					// add gun to joint
//					pGunSN->addComponent(hMyGunMesh);
//
//					// add gun scene node to the skin
//					pSkelInst->addComponent(hMyGunSN);
//				}
//#endif
//
//				pMainSN->addComponent(hSN);
//			}

			PE::Handle hMSN("SCENE_NODE", sizeof(SceneNode));
			SceneNode* pSN = new(hMSN) SceneNode(*m_pContext, m_arena, hMSN);
			pSN->addDefaultComponents();

			pSN->m_base.setPos(Vector3(0.0f, 0.f, 0.0f));

			PE::Handle hImrodMeshInst = PE::Handle("MeshInstance", sizeof(MeshInstance));
			MeshInstance* pImrodMeshInst = new(hImrodMeshInst) MeshInstance(*m_pContext, m_arena, hImrodMeshInst);

			pImrodMeshInst->addDefaultComponents();
			pImrodMeshInst->initFromFile("box.x_main_mesh.mesha", "Default", m_pContext->m_gameThreadThreadOwnershipMask);
			//pImrodMeshInst->setDrawBoundingBox(pSN, pSN);
			pImrodMeshInst->AddParticleEmitter();

            ParticleSetBufferGPU::initializeParticleMeshInstance(*m_pContext, pImrodMeshInst->m_hAsset);

			pSN->addComponent(hImrodMeshInst);

			//RootSceneNode::Instance()->addComponent(hSN);
			pMainSN->addComponent(hMSN);

			m_pContext->getGPUScreen()->ReleaseRenderContextOwnership(pEvt->m_threadOwnershipMask);

#if 1
			// add movement state machine to soldier npc
			PE::Handle hMove = PE::Handle("EnemyNPCMovementSM", sizeof(EnemyNPCMovementSM));
			EnemyNPCMovementSM* pMove = new(hMove) EnemyNPCMovementSM(*m_pContext, m_arena, hMove);
			pMove->addDefaultComponents();
			addComponent(hMove);

			//PE::Handle hSoldierMovementSM("SoldierNPCMovementSM", sizeof(SoldierNPCMovementSM));
			//SoldierNPCMovementSM* pSoldierMovementSM = new(hSoldierMovementSM) SoldierNPCMovementSM(*m_pContext, m_arena, hSoldierMovementSM);
			//pSoldierMovementSM->addDefaultComponents();

			//// add it to soldier NPC
			//addComponent(hSoldierMovementSM);

			// add behavior state machine ot soldier npc
			//PE::Handle hSoldierBheaviorSM("SoldierNPCBehaviorSM", sizeof(SoldierNPCBehaviorSM));
			//SoldierNPCBehaviorSM* pSoldierBehaviorSM = new(hSoldierBheaviorSM) SoldierNPCBehaviorSM(*m_pContext, m_arena, hSoldierBheaviorSM, hSoldierMovementSM);
			//pSoldierBehaviorSM->addDefaultComponents();

			//// add it to soldier NPC
			//addComponent(hSoldierBheaviorSM);

			//StringOps::writeToString(pEvt->m_patrolWayPoint, pSoldierBehaviorSM->m_curPatrolWayPoint, 32);
			//pSoldierBehaviorSM->m_havePatrolWayPoint = StringOps::length(pSoldierBehaviorSM->m_curPatrolWayPoint) > 0;

			// start the soldier
			//pSoldierBehaviorSM->start();
#endif
        }

        void TargetNPC::addDefaultComponents() {
            Component::addDefaultComponents();

            // custom methods of this component
        }
    }
}