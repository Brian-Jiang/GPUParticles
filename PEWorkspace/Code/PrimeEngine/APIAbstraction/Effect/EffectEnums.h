#ifndef __PYENGINE_2_0_EFFECT_ENUMS__
#define __PYENGINE_2_0_EFFECT_ENUMS__
// This class works with index buffers in GPU memory

// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

// Outer-Engine includes

// Inter-Engine includes

namespace EffectDrawOrder
{
	enum EffectDrawOrder_
	{
		First,
		Last,
		Manual,
	};
};

namespace EffectPSInputFamily
{
	enum EffectPSInputFamily_
	{
		MINIMAL_MESH_PS_IN,
		REDUCED_MESH_PS_IN,
		REDUCED_MESH_3DTC_PS_IN,
		STD_MESH_PS_IN,
		STD_MESH_SHADOWED_PS_IN,
		STD_MESH_BLEND_SHAPES_4_PS_IN,
		DETAILED_MESH_PS_IN,
		DETAILED_MESH_SHADOWED_PS_IN,
		REDUCED_SKIN_PS_IN,
		CUSTOM_REDUCED_MESH_F1,
		NO_PS,
		Count
	};
};


namespace PE
{

}
#endif
