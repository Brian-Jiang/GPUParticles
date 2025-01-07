#ifndef __PYENGINE_2_0_SPESHADER__
#define __PYENGINE_2_0_SPESHADER__
// This class works with index buffers in GPU memory

// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

// Outer-Engine includes

// Inter-Engine includes

#include "PrimeEngine/Render/IRenderer.h"
#include "PrimeEngine/Math/Matrix4x4.h"
#include "../Threading/Threading.h"
#include "../../Events/Component.h"
// Sibling/Children includes
#include "EffectEnums.h"


namespace PE {

struct SpeShader : public Components::Component
{
	PrimitiveTypes::UInt32 m_id;

	SpeShader(PE::GameContext &context, PE::MemoryArena arena, Handle hMyself) : Components::Component(context, arena, hMyself) {}
};

}; // namespace PE

#endif
