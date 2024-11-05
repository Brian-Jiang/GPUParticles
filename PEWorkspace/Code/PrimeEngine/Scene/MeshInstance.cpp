#define NOMINMAX
// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

// Outer-Engine includes

// Inter-Engine includes
#include "PrimeEngine/FileSystem/FileReader.h"
#include "PrimeEngine/APIAbstraction/GPUMaterial/GPUMaterialSet.h"
#include "PrimeEngine/PrimitiveTypes/PrimitiveTypes.h"
#include "PrimeEngine/APIAbstraction/Texture/Texture.h"
#include "PrimeEngine/APIAbstraction/Effect/EffectManager.h"
#include "PrimeEngine/APIAbstraction/GPUBuffers/VertexBufferGPUManager.h"
#include "PrimeEngine/Scene/Skeleton.h"
#include "PrimeEngine/Scene/ParticleEmitter.h"
#include "DefaultAnimationSM.h"
#include "Light.h"
#include "PrimeEngine/Scene/DebugRenderer.h"
#include "PrimeEngine/PrimitiveTypes/PrimitiveTypes.h"

#include "PrimeEngine/GameObjectModel/Camera.h"
#include "PrimeEngine/Physics/PhysicsManager.h"

// Sibling/Children includes
#include "MeshInstance.h"
#include "MeshManager.h"
#include "SceneNode.h"
#include "CameraManager.h"
#include "PrimeEngine/Lua/LuaEnvironment.h"
#include "PrimeEngine/Events/StandardEvents.h"

namespace PE {
namespace Components{

PE_IMPLEMENT_CLASS1(MeshInstance, Component);

MeshInstance::MeshInstance(PE::GameContext &context, PE::MemoryArena arena, Handle hMyself)
: Component(context, arena, hMyself)
, m_culledOut(false)
{
	
}

void MeshInstance::addDefaultComponents()
{
	Component::addDefaultComponents();

	PE_REGISTER_EVENT_HANDLER(Events::Event_PRE_RENDER_needsRC, MeshInstance::do_PRE_RENDER_needsRC);
}

void MeshInstance::initFromFile(const char *assetName, const char *assetPackage,
		int &threadOwnershipMask)
{
	Handle h = m_pContext->getMeshManager()->getAsset(assetName, assetPackage, threadOwnershipMask);

	initFromRegisteredAsset(h);

    // Add physics node
    {
        PE::Handle hPhysicsNode("Physics_Node", sizeof(PhysicsNode));
        PhysicsNode* pPhysicsNode = new(hPhysicsNode) PhysicsNode(*m_pContext, m_arena, hPhysicsNode, m_hMyself, true);
        int i = 1;
        pPhysicsNode->addDefaultComponents();
        //Sphere boundingSphere = pMeshInstance->getBoundingSphere();
        auto box = getBoundingBox();
        //pPhysicsNode->InitWithSphere(boundingSphere);
        pPhysicsNode->InitWithBox(box);
        m_hPhysicsNode = hPhysicsNode;

        PhysicsManager::Instance()->AddNode(hPhysicsNode);
    }
}

bool MeshInstance::hasSkinWeights()
{
	Mesh *pMesh = m_hAsset.getObject<Mesh>();
	return pMesh->m_hSkinWeightsCPU.isValid();
}

SceneNode* MeshInstance::getFirstParentSceneNode() {
    auto parent = getFirstParentByType<SceneNode>();
    if (!parent.isValid()) {
        parent = getFirstParentByTypePtr<Component>()->getFirstParentByType<SceneNode>();
    }

    return parent.getObject<SceneNode>();
}

void MeshInstance::do_PRE_RENDER_needsRC(PE::Events::Event* pEvt) {
    auto sceneNode = m_sceneNode.getObject<SceneNode>();
    if (!sceneNode) {
        // static mesh?
        //return;
        sceneNode = getFirstParentSceneNode();
    }

    auto transform = sceneNode->m_worldTransform;
    if (m_hPhysicsNode.isValid()) {
        m_hPhysicsNode.getObject<PhysicsNode>()->SyncTransform(transform);
    }

    if (!m_drawBoundingBox) return;

    updateBoundingBox();
    //auto parent = getFirstParentByType<SceneNode>();
    
    //auto parentTransform = getFirstParentSceneNode()->m_worldTransform;
    //m_boundingBox.m_transform = transform;
    //auto position = transform.getPos();
    //auto transform = Matrix4x4();
    //transform.loadIdentity();
    //transform.setU(m_boundingBox.m_directions[0]);
    //transform.setV(m_boundingBox.m_directions[1]);
    //transform.setN(m_boundingBox.m_directions[2]);
    //transform.setPos(parentTransform.getPos());
    //transform.setPos(m_boundingBox.m_position);
    DebugRenderer::Instance()->createLineBox(m_boundingBox, transform, Vector3(1.f, 1.f, 0.f), 2.f);

    
}

void MeshInstance::setDrawBoundingBox(const PE::Handle& sceneNodeHandle, const PE::Handle& mainSceneNodeHandle) {
    m_sceneNode = sceneNodeHandle;
    m_rootSceneNode = mainSceneNodeHandle;
    m_drawBoundingBox = true;

    m_boundingBox = getBoundingBox();
    updateBoundingBox();
}

void MeshInstance::setUseSphereForCollision() {
    auto sphere = getBoundingSphere();
    m_hPhysicsNode.getObject<PhysicsNode>()->InitWithSphere(sphere);
    m_hPhysicsNode.getObject<PhysicsNode>()->m_isStatic = false;
}

Box MeshInstance::getBoundingBox() {
    Mesh* pMesh = m_hAsset.getObject<Mesh>();
    Array<float>& positionValues = pMesh->m_hPositionBufferCPU.getObject<PositionBufferCPU>()->m_values;
    int size = positionValues.m_size / 3;
    auto vecMax = Vector3(-PrimitiveTypes::Constants::c_MaxFloat32, -PrimitiveTypes::Constants::c_MaxFloat32, -PrimitiveTypes::Constants::c_MaxFloat32);
    auto vecMin = Vector3(PrimitiveTypes::Constants::c_MaxFloat32, PrimitiveTypes::Constants::c_MaxFloat32, PrimitiveTypes::Constants::c_MaxFloat32);
    for (size_t i = 0; i < size; ++i) {
        auto x = positionValues[i * 3];
        auto y = positionValues[i * 3 + 1];
        auto z = positionValues[i * 3 + 2];
        vecMax.m_x = std::max(vecMax.m_x, x);
        vecMax.m_y = std::max(vecMax.m_y, y);
        vecMax.m_z = std::max(vecMax.m_z, z);
        vecMin.m_x = std::min(vecMin.m_x, x);
        vecMin.m_y = std::min(vecMin.m_y, y);
        vecMin.m_z = std::min(vecMin.m_z, z);
    }

    auto center = (vecMax + vecMin) / 2;
    auto extent = vecMax - vecMin;
    center.m_y += 0.1;
    Box result{ center, extent };
    return result;
}

void MeshInstance::updateBoundingBox() {
    auto transform = m_sceneNode.getObject<SceneNode>()->m_worldTransform;
    //m_boundingBox.m_transform = transform;
    //m_boundingBox.m_center = transform.getPos() + m_localCenter;
    transform.normalizeUVN();
    //m_boundingBox.m_directions[0] = transform.getU();
    //m_boundingBox.m_directions[1] = transform.getV();
    //m_boundingBox.m_directions[2] = transform.getN();
    //m_boundingBox.m_position = transform.getPos();
}

Sphere MeshInstance::getBoundingSphere() {
    Mesh* pMesh = m_hAsset.getObject<Mesh>();
    Array<float>& positionValues = pMesh->m_hPositionBufferCPU.getObject<PositionBufferCPU>()->m_values;
    int size = positionValues.m_size / 3;
    auto vecMax = Vector3(-PrimitiveTypes::Constants::c_MaxFloat32, -PrimitiveTypes::Constants::c_MaxFloat32, -PrimitiveTypes::Constants::c_MaxFloat32);
    auto vecMin = Vector3(PrimitiveTypes::Constants::c_MaxFloat32, PrimitiveTypes::Constants::c_MaxFloat32, PrimitiveTypes::Constants::c_MaxFloat32);
    for (size_t i = 0; i < size; ++i) {
        auto x = positionValues[i * 3];
        auto y = positionValues[i * 3 + 1];
        auto z = positionValues[i * 3 + 2];
        vecMax.m_x = std::max(vecMax.m_x, x);
        vecMax.m_y = std::max(vecMax.m_y, y);
        vecMax.m_z = std::max(vecMax.m_z, z);
        vecMin.m_x = std::min(vecMin.m_x, x);
        vecMin.m_y = std::min(vecMin.m_y, y);
        vecMin.m_z = std::min(vecMin.m_z, z);
    }

    auto center = (vecMax + vecMin) / 2;

    Sphere result{ center, 1.0f };
    
    return result;
}

void MeshInstance::MoveTo(Vector3 position) {
    m_hPhysicsNode.getObject<PhysicsNode>()->SetPosition(position);
}

void MeshInstance::FinalizePosition(Vector3 position) {
    if (!m_rootSceneNode.isValid()) {
        return;
    }

    auto sceneNode = m_rootSceneNode.getObject<SceneNode>();
    if (sceneNode) {
        sceneNode->m_base.setPos(position);
    }
    
}

void MeshInstance::AddParticleEmitter() {
    PE::Handle hParticleEmitter("Particle_Emitter", sizeof(ParticleEmitter));
    m_particleEmitter = new(hParticleEmitter) ParticleEmitter(*m_pContext, m_arena, hParticleEmitter);

    Mesh* pMesh = m_hAsset.getObject<Mesh>();
    pMesh->AddParticleEmitter();
}

void MeshInstance::initFromRegisteredAsset(const PE::Handle &h)
{
	m_hAsset = h;
	//add this instance as child to Mesh so that skin knows what to draw
	static int allowedEvts[] = {0};
	m_hAsset.getObject<Component>()->addComponent(m_hMyself, &allowedEvts[0]);
}


}; // namespace Components
}; // namespace PE
