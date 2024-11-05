#pragma once

#define NOMINMAX
// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

// Outer-Engine includes
#include <assert.h>

// Inter-Engine includes
#include "PrimeEngine/APIAbstraction/Effect/Effect.h"
#include "PrimeEngine/Math/Box.h"
#include "PrimeEngine/Math/Sphere.h"
#include "PrimeEngine/Scene/SceneNode.h"

namespace PE {
    namespace Components {
        class ParticleEmitter : public Component
        {
        public:
            PE_DECLARE_CLASS(ParticleEmitter);

            ParticleEmitter(PE::GameContext& context, PE::MemoryArena arena, Handle hMyself);
            virtual ~ParticleEmitter() {}

            virtual void addDefaultComponents();
        };
    }
}

