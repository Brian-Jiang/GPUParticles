#pragma once

#include "PrimeEngine/Events/Component.h"
#include "PrimeEngine/Physics/PhysicsNode.h"

namespace PE {
    namespace Components {
        struct PhysicsManager : public PE::Components::Component {
            PE_DECLARE_CLASS(PhysicsManager);

            PhysicsManager(PE::GameContext& context, PE::MemoryArena arena, Handle hMyself);

            virtual ~PhysicsManager() {}
            virtual void addDefaultComponents();

            PE_DECLARE_IMPLEMENT_EVENT_HANDLER_WRAPPER(do_Update_Physics);
            virtual void do_Update_Physics(Events::Event* pEvt);

            void AddNode(Handle hNode);

            static bool CalculateNodeCollision(PhysicsNode* node1, PhysicsNode* node2, const Vector3& direction, Vector3& outDirection);
            static bool DetectGroundCollision(PhysicsNode* node1, PhysicsNode* node2);

            static void Construct(PE::GameContext& context, PE::MemoryArena arena);
            static PhysicsManager* Instance() { return s_hInstance.getObject<PhysicsManager>(); }
            static void SetInstance(Handle h) { s_hInstance = h; }

        private:
            Array<Handle, 1> m_nodes;

            static Handle s_hInstance;
        };
    }
}
