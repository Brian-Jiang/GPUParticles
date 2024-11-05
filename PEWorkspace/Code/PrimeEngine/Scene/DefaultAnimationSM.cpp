

#define NOMINMAX
// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

// Outer-Engine includes

// Inter-Engine includes
#include "PrimeEngine/FileSystem/FileReader.h"
#include "PrimeEngine/APIAbstraction/GPUMaterial/GPUMaterialSet.h"
#include "PrimeEngine/APIAbstraction/Texture/Texture.h"
#include "PrimeEngine/APIAbstraction/Effect/EffectManager.h"
#include "../Geometry/SkeletonCPU/AnimationSetCPU.h"
#include "PrimeEngine/Scene/MeshInstance.h"
#include "PrimeEngine/Scene/SkeletonInstance.h"
#include "PrimeEngine/Scene/Skeleton.h"
#include "PrimeEngine/APIAbstraction/GPUBuffers/VertexBufferGPUManager.h"
#include "PrimeEngine/Scene/DebugRenderer.h"
#include "../Lua/LuaEnvironment.h"
#include "PrimeEngine/Geometry/SkeletonCPU/SkeletonCPU.h"
#include "PrimeEngine/APIAbstraction/GPUBuffers/AnimSetBufferGPU.h"
// Sibling/Children includes

#include "SceneNode.h"
#include "DrawList.h"
#include "SH_DRAW.h"

#include "DefaultAnimationSM.h"

int g_iDebugBoneSegment = -1;
int g_debugSkinning = 0;
int g_disableSkinRender = 0;
int g_debugSkinNormals = 0;
namespace PE {

namespace Components {

using namespace Events;

PE_IMPLEMENT_CLASS1(DefaultAnimationSM, Component);

DefaultAnimationSM::DefaultAnimationSM(PE::GameContext &context, PE::MemoryArena arena, Handle hMyself)
: Component(context, arena, hMyself)
, m_animSlots(context, arena)
, m_additionalLocalTransforms(context, arena)
, m_additionalLocalTransformFlags(context, arena)
, m_curPalette(context, arena)
, m_modelSpacePalette(context, arena)
, m_gpuAnimation(false)
, m_debugAnimIdOffset(0)
{
	// Animation slots ---------------------------------------------------------
	m_animSlots.reset(8);
	m_animSlots.add(AnimationSlot());
	m_animSlots.add(AnimationSlot());
	m_animSlots.add(AnimationSlot());
	m_animSlots.add(AnimationSlot());
	m_animSlots.add(AnimationSlot());
	m_animSlots.add(AnimationSlot());
	m_animSlots.add(AnimationSlot());
	m_animSlots.add(AnimationSlot());
}

void DefaultAnimationSM::addDefaultComponents()
{
	Component::addDefaultComponents(); // parent impl

	// add methods
	PE_REGISTER_EVENT_HANDLER(Events::Event_SCENE_GRAPH_UPDATE, DefaultAnimationSM::do_SCENE_GRAPH_UPDATE);
	if (!m_gpuAnimation)
		PE_REGISTER_EVENT_HANDLER(Events::Event_CALCULATE_TRANSFORMATIONS, DefaultAnimationSM::do_CALCULATE_TRANSFORMATIONS);
	// this event is used by PyClient/SkinViewer to set playing animation
	// but can be used by any other objects who want to set animation of the skin but don't know anything about its components (can't call methods directly)
	PE_REGISTER_EVENT_HANDLER(Events::Event_PLAY_ANIMATION, DefaultAnimationSM::do_Event_PLAY_ANIMATION);

	PE_REGISTER_EVENT_HANDLER(Event_PRE_RENDER_needsRC, DefaultAnimationSM::do_PRE_RENDER_needsRC);
}

// create the transfomrs so that they can be set to values
// additional transforms are applied after animations are blended
void DefaultAnimationSM::createAdditionalLocalTransforms()
{
	Handle hParentSkinInstance = getFirstParentByType<SkeletonInstance>();
	PEASSERT(hParentSkinInstance.isValid(), "SM has to belong to skeleton instance");
	
	SkeletonInstance *pSkelInstance = hParentSkinInstance.getObject<SkeletonInstance>();

	Skeleton *pSkeleton = pSkelInstance->getFirstParentByTypePtr<Skeleton>();
	// we can retrieve the skeleton
	// Additional Local Transforms (applied to joint transforms when these are calculated)
	SkeletonCPU *pSkel = pSkeleton->m_hSkeletonCPU.getObject<SkeletonCPU>();
	m_additionalLocalTransforms.reset(pSkel->m_numJoints);
	m_additionalLocalTransformFlags.reset(pSkel->m_numJoints);
	for (PrimitiveTypes::UInt32 ij = 0; ij < pSkel->m_numJoints; ij++)
	{
		m_additionalLocalTransforms.add(Matrix4x4());
		m_additionalLocalTransformFlags.add(false);
	}
}

void DefaultAnimationSM::do_SCENE_GRAPH_UPDATE(Events::Event *pEvt)
{
	Events::Event_UPDATE *pRealEvent = (Events::Event_UPDATE*)(pEvt);
	Handle hParentSkinInstance = getFirstParentByType<SkeletonInstance>();
	PEASSERT(hParentSkinInstance.isValid(), "SM has to belong to skeleton instance");

	SkeletonInstance *pSkelInstance = hParentSkinInstance.getObject<SkeletonInstance>();
	Skeleton *pSkeleton = pSkelInstance->getFirstParentByTypePtr<Skeleton>();

	SkeletonCPU *pSkelCPU = pSkeleton->m_hSkeletonCPU.getObject<SkeletonCPU>();
	if (m_curPalette.m_size == 0)
	{
		m_curPalette.reset(pSkelCPU->m_numJoints);
		m_modelSpacePalette.reset(pSkelCPU->m_numJoints);
		m_curPalette.m_size = m_modelSpacePalette.m_size = pSkelCPU->m_numJoints;
	}

	m_instanceCSJobIndex = -1; // reset this value
    //dbg
	//SkeletonCPU *pSkelCPU = pParentSkin->m_hSkeletonCPU.getObject<SkeletonCPU>();
	Array<Handle> &hAnimSets = pSkelInstance->m_hAnimationSetGPUs;

	if (hAnimSets.m_size == 0)
		return;

   
	// Go through each animation slot and increase a frame
	for (PrimitiveTypes::UInt32 iSlot = 0; iSlot < m_animSlots.m_size; iSlot++)
	{
		AnimationSlot &slot = m_animSlots[iSlot];
		if(!(slot.m_flags & ACTIVE))
		{
			continue;
		}

		PrimitiveTypes::UInt32 animIndex = slot.m_animationIndex;
		PrimitiveTypes::Float32 frame = slot.m_frameIndex;

		PrimitiveTypes::Float32 dFrame = pRealEvent->m_frameTime / 0.0333333f;

		// could scale speed here..
		//dFrame *= scale factor

		frame += dFrame;
		PrimitiveTypes::Float32 framesLeft = slot.m_framesLeft;
		framesLeft -= dFrame;

		AnimSetBufferGPU *pAnimSetBufferGPU = hAnimSets[slot.m_animationSetIndex].getObject<AnimSetBufferGPU>();
		AnimationSetCPU *pAnimSetCPU = pAnimSetBufferGPU->m_hAnimationSetCPU.getObject<AnimationSetCPU>();

		AnimationCPU &anim = pAnimSetCPU->m_animations[animIndex];

		bool doFrameCalculations = false;
		// if looping make sure that the last frame plays deserved time, i.e. 31 frames play frame_time * 31 where last frame is blended with 0th frame
		//if (frame >= (PrimitiveTypes::Float32)(anim.m_frames.m_size - (slot.m_looping ? 0 : 1)))

		if (framesLeft < 0.0f)
		{
			if (slot.isLooping() || slot.needToStayOnLastFrame())
			{
				if (slot.isLooping())
				{
					while (framesLeft < 0)
					{
						framesLeft += slot.m_numFrames;
						frame -= slot.m_numFrames;
					}
				}
				else
				{
					framesLeft = 0.5f;
					frame = slot.m_numFrames - 1.0f;
				}
				doFrameCalculations = true;
			}
			else
			{
				// end of animtion
				slot.setNotActive();
				if (slot.needToNotifyOnAnimationEnd())
				{
					Event_ANIMATION_ENDED evt;
					evt.m_hModel = this->m_hMyself;
					evt.m_animIndex = animIndex;
					Handle hComponentParent = getFirstParentByType<SceneNode>();
					if (hComponentParent.isValid())
					{
						hComponentParent.getObject<Component>()->handleEvent(&evt);
					}
					if (slot.m_framesLeft < 0)
					{
						// still < 0 -> nothing was done in the event handler
					}
					else
					{
						// the animation slot was reset -> TODO: need to apply the time difference
						doFrameCalculations = true;
						frame = slot.m_frameIndex;
						framesLeft = slot.m_framesLeft;
					}
				}
			}
		}
		else
		{
			if (slot.isFadingAway())
			{
				slot.m_weight -= slot.m_weight * (dFrame / slot.m_framesLeft);
			} else if (slot.needToNotifyOnAnimationEnd() && slot.m_framesLeft < slot.m_numFramesToNotifyBeforeEnd)
			{
				Event_ANIMATION_ENDED evt;
				evt.m_hModel = this->m_hMyself;
				evt.m_animIndex = animIndex;

				Handle hComponentParent = getFirstParentByType<SceneNode>();
				if (hComponentParent.isValid())
				{
					hComponentParent.getObject<Component>()->handleEvent(&evt);
				}
				if (slot.m_framesLeft < slot.m_numFramesToNotifyBeforeEnd)
				{
					// still < 0 -> nothing was done in the event handler
				}
				else
				{
					// the animation slot was reset -> TODO: need to apply the time difference
					doFrameCalculations = true;
					frame = slot.m_frameIndex;
					framesLeft = slot.m_framesLeft;
				}

			}

			doFrameCalculations = true;
		}

		if (doFrameCalculations)
		{
			slot.m_frameIndex = frame;
			slot.m_framesLeft = framesLeft;
			PrimitiveTypes::Float32 fframeIndex = floor(slot.m_frameIndex);
			PrimitiveTypes::Float32 alpha = slot.m_frameIndex - fframeIndex;
			PrimitiveTypes::UInt32 frameIndex0 = (PrimitiveTypes::UInt32)(fframeIndex);
			while (frameIndex0 >= anim.m_frames.m_size)
				frameIndex0 -= anim.m_frames.m_size;

			PrimitiveTypes::UInt32 frameIndex1 = frameIndex0 + 1 < anim.m_frames.m_size ? frameIndex0 + 1 : 0;
			slot.m_iFrameIndex0 = frameIndex0;
			slot.m_iFrameIndex1 = frameIndex1;
			slot.m_blendFactor = alpha;

			if (frameIndex0 >= anim.m_frames.m_size)
			{
				assert(0);
			}
		}
	}
}


void DefaultAnimationSM::do_CALCULATE_TRANSFORMATIONS(Events::Event *pEvt)
{
	Handle hParentSkinInstance = getFirstParentByType<SkeletonInstance>();
	PEASSERT(hParentSkinInstance.isValid(), "SM has to belong to skeleton instance");

	SkeletonInstance *pSkelInstance = hParentSkinInstance.getObject<SkeletonInstance>();
	Skeleton *pSkeleton = pSkelInstance->getFirstParentByTypePtr<Skeleton>();

	SkeletonCPU *pSkelCPU = pSkeleton->m_hSkeletonCPU.getObject<SkeletonCPU>();
	if (m_curPalette.m_size == 0)
	{
		m_curPalette.reset(pSkelCPU->m_numJoints);
		m_modelSpacePalette.reset(pSkelCPU->m_numJoints);
		m_curPalette.m_size = m_modelSpacePalette.m_size = pSkelCPU->m_numJoints;
	}

	if ( !m_gpuAnimation)
	{
		bool haveAnim = false;
		
		for (PrimitiveTypes::UInt32 iSlot = 0; iSlot < m_animSlots.m_size; iSlot++)
		{
			if (m_animSlots[iSlot].m_flags & ACTIVE)
			{
				haveAnim = !m_gpuAnimation;
				break;
			}
		}
		
		if (haveAnim)
		{
			pSkelCPU->prepareMatrixPalette(pSkelInstance->m_hAnimationSetGPUs, m_animSlots,
				m_additionalLocalTransforms, m_additionalLocalTransformFlags,
				m_modelSpacePalette);

			Matrix4x4 scale;
			scale.importScale(pSkeleton->m_skinScaleFactor, pSkeleton->m_skinScaleFactor, pSkeleton->m_skinScaleFactor);

			for (PrimitiveTypes::UInt32 ijoint = 0; ijoint < m_modelSpacePalette.m_size; ijoint++)
			{
				m_modelSpacePalette[ijoint] = scale * m_modelSpacePalette[ijoint];
			}
		}
		else
		{
			// load bind pose
			pSkelCPU->prepareBindPoseMatrixPalette(m_modelSpacePalette, true);
		}

		Matrix4x4 tmp;
		tmp.loadIdentity();

		Handle hp = getFirstParentByType<SceneNode>();
		if (hp.isValid())
		{	
			tmp = hp.getObject<PE::Components::SceneNode>()->m_base;
		}

		// finally add inverse transformation of vertices into local space of the bones (bind pose transformation)
		// need to apply it, because vertices are stored as if they were a simple mesh (that is in bind pose)
		// so we need to get them into bone space (by multiplying by bind pose joint inverse)
		// and then move with joint (by multiplying by joint transformation)
		PEASSERT(m_modelSpacePalette.m_size == m_curPalette.m_size, "Palettes must be same size");
		pSkelCPU->applyInverses(m_curPalette.getFirstPtr(), m_modelSpacePalette.getFirstPtr());
	}	
}

// this event is executed when thread has RC
void DefaultAnimationSM::do_PRE_RENDER_needsRC(PE::Events::Event *pEvt)
{
	Event_PRE_RENDER_needsRC *pRealEvt = (Event_PRE_RENDER_needsRC *)(pEvt);
    SceneNode *pSN = getFirstParentByTypePtr<SkeletonInstance>()->getFirstParentByTypePtr<SceneNode>();
    
	static bool debugSkeleton = true;
	if (!debugSkeleton)
		return;

	SkeletonInstance *pSkelInst = getFirstParentByTypePtr<SkeletonInstance>();
	Skeleton *pSkel = pSkelInst->getFirstParentByTypePtr<Skeleton>();
	SkeletonCPU *pSkelCPU = pSkel->m_hSkeletonCPU.getObject<SkeletonCPU>();

#if APIABSTRACTION_D3D11
	if (m_instanceCSJobIndex != -1)
	{
		Matrix4x4 *pResultCSMem = (Matrix4x4 *)(AnimSetBufferGPU::s_pCSReduceComputeTargetCpuMem);
		Matrix4x4 *pPalette = &pResultCSMem[m_instanceCSJobIndex * m_modelSpacePalette.m_size];

		// the result already has bind inverse multiplied in, so we need to undo it

		pSkelCPU->prepareBindPoseMatrixPalette(m_modelSpacePalette, true);

		for (int i = 0; i < m_modelSpacePalette.m_size; ++i)
		{
			m_modelSpacePalette[i] = pPalette[i] * m_modelSpacePalette[i];
		}
	}
#endif
	// render skeleton
	
	float scale = 100.0f;
	if (g_iDebugBoneSegment < 0 && !g_debugSkinning)
	{
		for (int i = 0; i < m_modelSpacePalette.m_size; ++i)
		{
			Matrix4x4 m = m_modelSpacePalette[i];
			m = pSN->m_worldTransform * m;
			m.setU(m.getU() * scale);
			m.setV(m.getV() * scale);
			m.setN(m.getN() * scale);

			DebugRenderer::Instance()->createLineMesh(true, m, NULL, 0, 0, 0.2f);
		}
	}

	// test out skinning
	int index = -1;
	MeshInstance *pMeshInst = NULL;
	static bool normalsForDebugVertexOnly = false;
	static float normalScale = 0.1f;
	while (pSkelInst->getFirstComponentIP<MeshInstance>(index+1, index, pMeshInst))
	{
		Mesh *pMesh = pMeshInst->getFirstParentByTypePtr<Mesh>();
		SkinWeightsCPU *pWeights = pMesh->m_hSkinWeightsCPU.getObject<SkinWeightsCPU>();
		PositionBufferCPU *pPoss = pMesh->m_hPositionBufferCPU.getObject<PositionBufferCPU>();
		NormalBufferCPU *pNorms = pMesh->m_hNormalBufferCPU.getObject<NormalBufferCPU>();
		TangentBufferCPU *pTangs = pMesh->m_hTangentBufferCPU.getObject<TangentBufferCPU>();
		IndexBufferGPU *pIB = pMesh->m_hIndexBufferGPU.getObject<IndexBufferGPU>();
		
		for (int iIndexRange = 0; iIndexRange < pIB->m_indexRanges.m_size; ++iIndexRange)
		{
			IndexRange &ir = pIB->m_indexRanges[iIndexRange];
			if (g_iDebugBoneSegment >= 0 && g_iDebugBoneSegment < ir.m_boneSegments.m_size)
			{
				IndexRange::BoneSegment &bs = ir.m_boneSegments[g_iDebugBoneSegment];
				
				for (int ibone = 0; ibone < (int)(bs.m_boneSegmentBones.m_size > PE_MAX_BONE_COUNT_IN_DRAW_CALL ? PE_MAX_BONE_COUNT_IN_DRAW_CALL : bs.m_boneSegmentBones.m_size); ibone++)
				{
					int iJoint = bs.m_boneSegmentBones[ibone];
					Matrix4x4 m = m_modelSpacePalette[iJoint];
					m = pSN->m_worldTransform * m;
					m.setU(m.getU() * scale);
					m.setV(m.getV() * scale);
					m.setN(m.getN() * scale);

					DebugRenderer::Instance()->createLineMesh(true, m, NULL, 0, 0, 0.2f);
				}
			}
		}


		// debug skinning

		if (g_debugSkinning)
		{
			static float fVertexIndex = 0;
			static float fInc = 0.5f;
			fVertexIndex += fInc;

			static int iVertexIndex = 1150;
			int vertexIndex = iVertexIndex >= 0 ? iVertexIndex : int(fVertexIndex);

			vertexIndex = vertexIndex % (pPoss->m_values.m_size / 3);

			Array<float> arr(*m_pContext, m_arena);
			arr.reset(pNorms->m_values.m_size/3 * 2 * 6 * 2);

			for (int i = 0; i < pPoss->m_values.m_size / 3; ++i)
			{
				Vector3 pos(pPoss->m_values[i * 3], pPoss->m_values[i * 3 + 1], pPoss->m_values[i * 3 + 2]);
				Vector3 normal(pNorms->m_values[i * 3], pNorms->m_values[i * 3 + 1], pNorms->m_values[i * 3 + 2]);
				Vector3 tangent(pTangs->m_values[i * 3], pTangs->m_values[i * 3 + 1], pTangs->m_values[i * 3 + 2]);


				if (i == vertexIndex)
				{
					// this is the vertex we want to debug
					Vector3 posW = pSN->m_worldTransform * pos;

					char buf[256];
					sprintf(buf, "%d:[%.2f,%.2f,%.2f]", vertexIndex, posW.m_x, posW.m_y, posW.m_z);
					DebugRenderer::Instance()->createTextMesh(buf, false, true, true, false, 0.0f, posW, 0.5f, pRealEvt->m_threadOwnershipMask);

					Matrix4x4 m(posW);

					DebugRenderer::Instance()->createLineMesh(true, m, NULL, 0, 0, 0.1f);
				}

				Vector3 res(0,0,0);
				Vector3 normRes(0,0,0);
				Vector3 tangRes(0,0,0);

				Array<WeightPair> &weights = pWeights->m_weightsPerVertex[i];
				for (int iw = 0; iw < weights.m_size; ++iw)
				{
					WeightPair &w = weights[iw];

					Matrix4x4 m = m_modelSpacePalette[w.m_jointIndex];

					Matrix4x4 inv = pSkelCPU->getBindInversesPtr()[w.m_jointIndex];
					Matrix4x4 mInv = m * inv;
					Vector3 invTimesPos = inv * pos;

					res = res + (mInv * pos) * w.m_weight;
					normRes = normRes + (mInv * normal) * w.m_weight;
					tangRes = tangRes + (mInv * tangent) * w.m_weight;

					if (i == vertexIndex)
					{
						// this joint is used by debugged vertex
						m = pSN->m_worldTransform * m;
						m.setU(m.getU() * 100.0f);
						m.setV(m.getV() * 100.0f);
						m.setN(m.getN() * 100.0f);

						DebugRenderer::Instance()->createLineMesh(true, m, NULL, 0, 0, 0.1f);

						char buf[256];
						sprintf(buf, "%d:%d-%.2f", iw, w.m_jointIndex, w.m_weight);
						DebugRenderer::Instance()->createTextMesh(buf, false, true, true, false, 0.0f, m.getPos(), 0.5f, pRealEvt->m_threadOwnershipMask);
					}


				}

				if (g_debugSkinNormals && (!normalsForDebugVertexOnly || i == vertexIndex))
				{
					arr.add(res.m_x, res.m_y, res.m_z);
					arr.add(0.0f, 0.0f, 1.0f);

					arr.add(res.m_x + normRes.m_x * normalScale, res.m_y + normRes.m_y * normalScale, res.m_z + normRes.m_z * normalScale);
					arr.add(0.0f, 0.0f, 1.0f);

					arr.add(res.m_x, res.m_y, res.m_z);
					arr.add(1.0f, 0.0f, 0.0f);

					arr.add(res.m_x + tangRes.m_x * normalScale, res.m_y + tangRes.m_y * normalScale, res.m_z + tangRes.m_z * normalScale);
					arr.add(1.0f, 0.0f, 0.0f);
				}

				if (i == vertexIndex)
				{
					// render final result of debugged vertex
					Vector3 resW = pSN->m_worldTransform * res;

					char buf[256];
					sprintf(buf, "%d:[%.2f,%.2f,%.2f]", vertexIndex, resW.m_x, resW.m_y, resW.m_z);
					DebugRenderer::Instance()->createTextMesh(buf, false, true, true, false, 0.0f, resW, 0.5f, pRealEvt->m_threadOwnershipMask);

					Matrix4x4 m(resW);

					DebugRenderer::Instance()->createLineMesh(true, m, NULL, 0, 0, 0.2f);
				}
			}
			DebugRenderer::Instance()->createLineMesh(false, Matrix4x4(), arr.getFirstPtr(), arr.m_size/6, 0, 1.0f);
			arr.reset(0); // clear memory
		}
	}
}

//
void DefaultAnimationSM::do_Event_PLAY_ANIMATION(Events::Event *pEvt)
{
	Events::Event_PLAY_ANIMATION *pRealEvent = (Events::Event_PLAY_ANIMATION *)(pEvt);
	setAnimation(pRealEvent->m_animSetIndex, pRealEvent->m_animIndex,
		0, 0, 1, 1,
		LOOPING);
}

AnimationSlot *DefaultAnimationSM::setAnimation(
	PrimitiveTypes::UInt32 animationSetIndex,
	PrimitiveTypes::UInt32 animationIndex,
	PrimitiveTypes::UInt32 firstActiveAnimSlotIndex,
	PrimitiveTypes::UInt32 lastActiveAnimSlotIndex,
	PrimitiveTypes::UInt32 firstFadingAnimSlotIndex,
	PrimitiveTypes::UInt32 lastFadingAnimSlotIndex,
	PrimitiveTypes::UInt32 additionalFlags,
	PrimitiveTypes::Float32 weight,
	Array<PrimitiveTypes::UInt32> *animationsNotToExclude /* = NULL*/,
	PrimitiveTypes::Bool useProgressPercentageOfReplacedAnimation,
	PrimitiveTypes::Float32 customPercentage)
{
	Handle hParentSkinInstance = getFirstParentByType<SkeletonInstance>();
	PEASSERT(hParentSkinInstance.isValid(), "SM has to belong to skeleton instance");

	SkeletonInstance *pSkelInstance = hParentSkinInstance.getObject<SkeletonInstance>();
	Skeleton *pSkeleton = pSkelInstance->getFirstParentByTypePtr<Skeleton>();

	SkeletonCPU *pSkelCPU = pSkeleton->m_hSkeletonCPU.getObject<SkeletonCPU>();

	PrimitiveTypes::UInt32 goodSlot = firstActiveAnimSlotIndex;
	PrimitiveTypes::Float32 goodPercentage = customPercentage;
	for (PrimitiveTypes::UInt32 i = firstActiveAnimSlotIndex; i <= lastActiveAnimSlotIndex; i++)
	{
		AnimationSlot slot = m_animSlots[i];
		if (slot.isActive() )
		{
			bool isExcluded = false;
			for (PrimitiveTypes::UInt32 inex = 0; inex < (animationsNotToExclude ? animationsNotToExclude->m_size : 0); inex++)
			{
				if (slot.m_animationIndex == (*animationsNotToExclude)[inex])
				{
					isExcluded = true;
					break;
				}
			}
			if (isExcluded)
			{
				if (goodSlot == i)
					goodSlot++;
				continue;
			}
			// some animation is running, need to make it fade
			slot.m_framesLeft = min(slot.m_numFrames, 30.0f);
			slot.setNotLooping();
			if (slot.isLooping())
			{
				assert(0);
			}
			slot.setNotToNotifyOnAnimationEnd();
			slot.setFadingAway();
			slot.m_weight = 30.0f;

			// now need to put in one of the fading slots
			bool found = false;
			for (PrimitiveTypes::UInt32 j = firstFadingAnimSlotIndex; j <= lastFadingAnimSlotIndex; j++)
			{
				AnimationSlot &repSlot = m_animSlots[j];

				// skip if it is still active. can't skip if the last one. chose last one even if active since we need to put the animation in some slot
				if (m_animSlots[j].isActive() && j != lastFadingAnimSlotIndex)
					continue;
				setSlot(j, slot);
				if (repSlot.isLooping())
				{
					assert(!"Found an animation in a fading slot that is looping!");
				}
				found = true;
				break;
			}
			if (!found)
			{
				assert(!"Could not put a new fading slot");
			}
			slot.setNotActive(); // this is now a not active slot since we moved the animation to fading slot
			goodSlot = i; // this slot will be used for the new animation
			if (useProgressPercentageOfReplacedAnimation)
				goodPercentage = slot.getProgressPercentage();
		}
	}

	assert(firstActiveAnimSlotIndex <= goodSlot && goodSlot <= lastActiveAnimSlotIndex);
	AnimSetBufferGPU *pAnimSetBufferGPU = pSkelInstance->m_hAnimationSetGPUs[animationSetIndex].getObject<AnimSetBufferGPU>();
	AnimationSetCPU *pAnimSet = pAnimSetBufferGPU->m_hAnimationSetCPU.getObject<AnimationSetCPU>();
	AnimationCPU &anim = pAnimSet->m_animations[(animationIndex + m_debugAnimIdOffset)];
	AnimationSlot slot(animationSetIndex, (animationIndex + m_debugAnimIdOffset), 0, (PrimitiveTypes::Float32)(anim.m_frames.m_size-1), anim.m_startJoint, anim.m_endJoint, ACTIVE | additionalFlags /*| PARTIAL_BODY_ANIMATION | NOTIFY_ON_ANIMATION_END*/, weight);
	setSlot(goodSlot, slot);
	return &m_animSlots[goodSlot];
}

void DefaultAnimationSM::setAnimations(
	PrimitiveTypes::UInt32 animationSetIndex0,
	PrimitiveTypes::UInt32 animationIndex0,
	PrimitiveTypes::UInt32 animationSetIndex1,
	PrimitiveTypes::UInt32 animationIndex1,
	PrimitiveTypes::Float32 alpha,
	PrimitiveTypes::UInt32 firstActiveAnimSlotIndex,
	PrimitiveTypes::UInt32 lastActiveAnimSlotIndex,
	PrimitiveTypes::UInt32 firstFadingAnimSlotIndex,
	PrimitiveTypes::UInt32 lastFadingAnimSlotIndex,
	PrimitiveTypes::UInt32 additionalFlags)
{
	setAnimation(animationSetIndex0, animationIndex0,
		firstActiveAnimSlotIndex, lastActiveAnimSlotIndex,
		firstFadingAnimSlotIndex, lastFadingAnimSlotIndex,
		additionalFlags,
		1.0f - alpha);

	Array<PrimitiveTypes::UInt32> animationsNotToExclude(*m_pContext, m_arena, 1);
	animationsNotToExclude.add(animationIndex0);

	setAnimation(animationSetIndex1, animationIndex1,
		firstActiveAnimSlotIndex, lastActiveAnimSlotIndex,
		firstFadingAnimSlotIndex, lastFadingAnimSlotIndex,
		additionalFlags,
		alpha,
		&animationsNotToExclude);

	animationsNotToExclude.reset(0);
}

PrimitiveTypes::Int32 DefaultAnimationSM::setAnimationWeight(
	PrimitiveTypes::UInt32 animationSetIndex,
	PrimitiveTypes::UInt32 animationIndex,
	PrimitiveTypes::UInt32 firstActiveAnimSlotIndex,
	PrimitiveTypes::UInt32 lastActiveAnimSlotIndex,
	PrimitiveTypes::Float32 weight)
{
	PrimitiveTypes::Int32 set = 0;
	for (PrimitiveTypes::UInt32 islot = firstActiveAnimSlotIndex; islot <= lastActiveAnimSlotIndex; islot++)
	{
		AnimationSlot &slot = m_animSlots[islot];
		if (slot.isActive())
		{
			if (slot.m_animationSetIndex == animationSetIndex && slot.m_animationIndex == animationIndex)
			{
				slot.m_weight = weight;
				set++;
			}
		}
	}
	return set;
}

PrimitiveTypes::Int32 DefaultAnimationSM::setWeightsBetweenAnimations(
	PrimitiveTypes::UInt32 animationSetIndex0,
	PrimitiveTypes::UInt32 animationIndex0,
	PrimitiveTypes::UInt32 animationSetIndex1,
	PrimitiveTypes::UInt32 animationIndex1,
	PrimitiveTypes::UInt32 firstActiveAnimSlotIndex,
	PrimitiveTypes::UInt32 lastActiveAnimSlotIndex,
	PrimitiveTypes::Float32 alpha)
{
	PrimitiveTypes::Int32 set = 0;
	set += setAnimationWeight(animationSetIndex0, animationIndex0,
		firstActiveAnimSlotIndex, lastActiveAnimSlotIndex,
		1.0f - alpha);

	set += setAnimationWeight(animationIndex1, animationIndex1,
		firstActiveAnimSlotIndex, lastActiveAnimSlotIndex,
		alpha);
	return set;
}



AnimationSlot *DefaultAnimationSM::getSlot(
	PrimitiveTypes::UInt32 animationSetIndex,
	PrimitiveTypes::UInt32 animationIndex,
	PrimitiveTypes::UInt32 firstActiveAnimSlotIndex,
	PrimitiveTypes::UInt32 lastActiveAnimSlotIndex)
{
	for (PrimitiveTypes::UInt32 islot = firstActiveAnimSlotIndex; islot <= lastActiveAnimSlotIndex; islot++)
	{
		AnimationSlot &slot = m_animSlots[islot];
		if (slot.isActive())
		{
			if (slot.m_animationSetIndex == animationSetIndex && slot.m_animationIndex == animationIndex)
			{
				return &slot;
			}
		}
	}
	return NULL;
}


PrimitiveTypes::Float32 DefaultAnimationSM::getProgressPercentage(
	PrimitiveTypes::UInt32 animationSetIndex,
	PrimitiveTypes::UInt32 animationIndex,
	PrimitiveTypes::UInt32 firstActiveAnimSlotIndex,
	PrimitiveTypes::UInt32 lastActiveAnimSlotIndex)
{
	AnimationSlot *pslot = getSlot(animationSetIndex, animationIndex, firstActiveAnimSlotIndex, lastActiveAnimSlotIndex);
	if (!pslot)
		assert(!"DefaultAnimationSM::getProgressPercentage(): did not find a slot!");
	return pslot->getProgressPercentage();
}

PrimitiveTypes::Bool DefaultAnimationSM::isRunningAnimation(
	PrimitiveTypes::UInt32 animationSetIndex,
	PrimitiveTypes::UInt32 animationIndex,
	PrimitiveTypes::UInt32 firstActiveAnimSlotIndex,
	PrimitiveTypes::UInt32 lastActiveAnimSlotIndex)
{
	AnimationSlot *pslot = getSlot(animationSetIndex, animationIndex, firstActiveAnimSlotIndex, lastActiveAnimSlotIndex);
	return pslot != NULL;
}

void DefaultAnimationSM::disableAnimation(
	PrimitiveTypes::UInt32 animationSetIndex,
	PrimitiveTypes::UInt32 animationIndex,
	PrimitiveTypes::UInt32 firstActiveAnimSlotIndex,
	PrimitiveTypes::UInt32 lastActiveAnimSlotIndex)
{
	AnimationSlot *pslot = getSlot(animationSetIndex, animationIndex, firstActiveAnimSlotIndex, lastActiveAnimSlotIndex);
	//if (!slot.isActive())
	//	assert(!"Skin::disableAnimation(): did not find a slot!");
	if (pslot)
		pslot->setNotActive();
}

void DefaultAnimationSM::disableAnimations(
	PrimitiveTypes::UInt32 firstActiveAnimSlotIndex,
	PrimitiveTypes::UInt32 lastActiveAnimSlotIndex)
{
	for (PrimitiveTypes::UInt32 islot = firstActiveAnimSlotIndex; islot <= lastActiveAnimSlotIndex; islot++)
	{
		AnimationSlot &slot = m_animSlots[islot];
		if (slot.isActive())
		{
			slot.setNotActive();
		}
	}
}

void DefaultAnimationSM::fadeAnimation(
	PrimitiveTypes::UInt32 animationSetIndex,
	PrimitiveTypes::UInt32 animationIndex,
	PrimitiveTypes::UInt32 firstActiveAnimSlotIndex,
	PrimitiveTypes::UInt32 lastActiveAnimSlotIndex,
	PrimitiveTypes::UInt32 firstFadingAnimSlotIndex,
	PrimitiveTypes::UInt32 lastFadingAnimSlotIndex)
{
	AnimationSlot *pslot = getSlot(animationSetIndex, (animationIndex+m_debugAnimIdOffset), firstActiveAnimSlotIndex, lastActiveAnimSlotIndex);
	if (pslot)
	{
		// some animation is running, need to make it fade
		pslot->m_framesLeft = min(pslot->m_numFrames, 30.0f);
		pslot->setNotLooping();
		pslot->setNotToNotifyOnAnimationEnd();
		pslot->setFadingAway();
		pslot->m_weight = 30.0f;

		// now need to put in one of the fading slots
		bool found = false;
		for (PrimitiveTypes::UInt32 j = firstFadingAnimSlotIndex; j <= lastFadingAnimSlotIndex; j++)
		{
			// skip if it is still active. can't skip if the last one. chose last one even if active since we need to put the animation in some slot
			if (m_animSlots[j].isActive() && j != lastFadingAnimSlotIndex)
				continue;
			setSlot(j, *pslot);
			pslot->setNotActive();

			found = true;
			break;
		}
		if (!found)
		{
			assert(!"Could not put a new fading slot");
		}
	}
}


}; // namespace Components
}; // namespace PE
