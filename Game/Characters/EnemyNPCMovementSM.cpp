#include "EnemyNPCMovementSM.h"
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"
#include "PrimeEngine/Lua/LuaEnvironment.h"

using namespace PE::Components;
using namespace PE::Events;
using namespace CharacterControl::Events;

namespace CharacterControl {
    namespace Components {
        PE_IMPLEMENT_CLASS1(EnemyNPCMovementSM, Component);

        EnemyNPCMovementSM::EnemyNPCMovementSM(PE::GameContext& context, PE::MemoryArena arena, PE::Handle hMyself) :
            Component(context, arena, hMyself), cumulativeTime(0.f) { }

        void EnemyNPCMovementSM::addDefaultComponents() {
            Component::addDefaultComponents();

            PE_REGISTER_EVENT_HANDLER(Event_UPDATE, EnemyNPCMovementSM::do_UPDATE);
        }

        SceneNode* EnemyNPCMovementSM::getParentsSceneNode() {
            PE::Handle hParent = getFirstParentByType<Component>();
            if (hParent.isValid()) {
                // see if parent has scene node component
                return hParent.getObject<Component>()->getFirstComponent<SceneNode>();

            }
            return NULL;
        }

        void EnemyNPCMovementSM::do_UPDATE(PE::Events::Event* pEvt) {
            SceneNode* pSN = getParentsSceneNode();
            if (!pSN) return;

            Event_UPDATE* pRealEvt = (Event_UPDATE*)(pEvt);
            cumulativeTime += pRealEvt->m_frameTime;

            static Vector3 center = Vector3(-10.f, 0, 0);
            static float radius = 5.0f;
            static float speed = 0.6f;
            //float allowedDisp = speed * pRealEvt->m_frameTime;
            Vector3 curPos = pSN->m_base.getPos();
            Vector3 position;
            position.m_x = center.m_x + radius * cos(cumulativeTime * speed);
            position.m_y = curPos.m_y;
            position.m_z = center.m_z + radius * sin(cumulativeTime * speed);

            Vector3 dir = curPos - position;
            pSN->m_base.turnInDirection(dir, 3.1415f);
            pSN->m_base.setPos(position);
        }


    }
}