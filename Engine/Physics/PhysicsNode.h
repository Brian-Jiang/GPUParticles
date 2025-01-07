#pragma once

#include "PrimeEngine/Events/Component.h"
#include "PrimeEngine/Math/Sphere.h"
#include "PrimeEngine/Math/Box.h"
#include "PrimeEngine/Math/Matrix4x4.h"

namespace PE {
    namespace Components {
        enum PhysicsNodeType {
            PNUndefined,
            PNSphere,
            PNBox
        };

        struct PhysicsNode : public PE::Components::Component {
            PE_DECLARE_CLASS(PhysicsNode);

            PhysicsNode(PE::GameContext& context, PE::MemoryArena arena, Handle hMyself, Handle hMeshInstance, bool isStatic = false);

            virtual ~PhysicsNode() {}

            virtual void addDefaultComponents();

            void InitWithSphere(Sphere boundingSphere);
            void InitWithBox(Box boundingBox);

            void SetPosition(Vector3 position);
            void FinalizePosition(Vector3 position);

            void SyncTransform(const Matrix4x4& transform);

        //private:
            PhysicsNodeType m_type;
            Sphere m_boundingSphere;
            Box m_boundingBox;

            Vector3 m_currentPosition;
            Vector3 m_targetPosition;

            Matrix4x4 m_transform;

            bool m_isStatic;
            Handle m_hMeshInstance;
        };
    }
}