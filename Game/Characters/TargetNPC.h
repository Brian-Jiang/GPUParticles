#pragma once
#include "PrimeEngine/Events/Component.h"

#include "CharacterControl/Events/Events.h"

namespace CharacterControl {
    namespace Components {

        struct TargetNPC : public PE::Components::Component
        {
            PE_DECLARE_CLASS(TargetNPC);

            TargetNPC(PE::GameContext& context, PE::MemoryArena arena, PE::Handle hMyself, Events::Event_CreateTargetNPC* pEvt);

            virtual void addDefaultComponents();
        };

    }
}