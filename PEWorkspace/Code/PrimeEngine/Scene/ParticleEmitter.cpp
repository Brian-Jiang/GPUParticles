#include "ParticleEmitter.h"

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
#include "PrimeEngine/../../GlobalConfig/GlobalConfig.h"

#include "PrimeEngine/Geometry/SkeletonCPU/SkeletonCPU.h"

#include "PrimeEngine/Scene/RootSceneNode.h"

#include "Light.h"

// Sibling/Children includes

#include "SceneNode.h"
#include "DrawList.h"
#include "SH_DRAW.h"
#include "PrimeEngine/Lua/LuaEnvironment.h"

namespace PE {
    namespace Components {

        PE_IMPLEMENT_CLASS1(ParticleEmitter, Component);

        ParticleEmitter::ParticleEmitter(PE::GameContext& context, PE::MemoryArena arena, Handle hMyself)
            : Component(context, arena, hMyself) {}

        void ParticleEmitter::addDefaultComponents() {
            Component::addDefaultComponents();

        }

    }
}