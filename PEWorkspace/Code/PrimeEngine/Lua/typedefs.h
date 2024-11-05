#ifndef __PYENGINE_2_0_LUA_TYPEDEFS_H__
#define __PYENGINE_2_0_LUA_TYPEDEFS_H__

// this file defines lua glue functions that create event data
// these functions will be called from lua to create event data


// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

// Outer-Engine includes
#include <assert.h>

// Inter-Engine includes

namespace PE {
namespace LuaGlue
{
typedef int LuaReference;
};
};


#endif
