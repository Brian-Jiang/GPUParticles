#pragma once

#include "PrimeEngine/Events/Component.h"
#include "PrimeEngine/Scene/SceneNode.h"

#include "CharacterControl/Events/Events.h"

namespace CharacterControl {
namespace Components {

struct EnemyNPCMovementSM : public PE::Components::Component
{
    PE_DECLARE_CLASS(EnemyNPCMovementSM);

    EnemyNPCMovementSM(PE::GameContext& context, PE::MemoryArena arena, PE::Handle hMyself);

    virtual void addDefaultComponents();
    PE::Components::SceneNode* getParentsSceneNode();

    PE_DECLARE_IMPLEMENT_EVENT_HANDLER_WRAPPER(do_UPDATE)
    virtual void do_UPDATE(PE::Events::Event* pEvt);

private:
    float cumulativeTime;
};

}
}