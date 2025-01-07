#include "PhysicsNode.h"
#include "PrimeEngine/Lua/LuaEnvironment.h"
#include "PrimeEngine/Scene/MeshInstance.h"

namespace PE {
    namespace Components {
        PE_IMPLEMENT_CLASS1(PhysicsNode, Component);

        PhysicsNode::PhysicsNode(PE::GameContext& context, PE::MemoryArena arena, Handle hMyself, Handle hMeshInstance, bool isStatic)
            : Component(context, arena, hMyself), m_hMeshInstance(hMeshInstance), m_isStatic(isStatic)
        {
            if (!m_hMeshInstance.isValid()) {
                m_hMeshInstance = Handle(hMeshInstance);
            }
        }

        void PhysicsNode::addDefaultComponents() {
            Component::addDefaultComponents();


        }

        void PhysicsNode::InitWithSphere(Sphere boundingSphere) {
            m_boundingSphere = boundingSphere;
            m_type = PNSphere;
        }

        void PhysicsNode::InitWithBox(Box boundingBox) {
            m_boundingBox = boundingBox;
            m_type = PNBox;
        }

        void PhysicsNode::SetPosition(Vector3 position) {
            m_targetPosition = position;
        }

        void PhysicsNode::SyncTransform(const Matrix4x4& transform) {
            m_transform = transform;
        }

        void PhysicsNode::FinalizePosition(Vector3 position) {
            m_currentPosition = position;
            m_hMeshInstance.getObject<MeshInstance>()->FinalizePosition(position);
        }
    }
}