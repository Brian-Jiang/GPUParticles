#ifndef __PYENGINE_2_0_INPUT_H___
#define __PYENGINE_2_0_INPUT_H___

// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"


// Outer-Engine includes
#include <assert.h>

// Inter-Engine includes
#include "PrimeEngine/MemoryManagement/Handle.h"

#include "../../Events/Component.h"

#if APIABSTRACTION_D3D9 || APIABSTRACTION_D3D11
#include "../DirectX9/DX9_XInput/DX9_XInput.h"
#elif PE_PLAT_IS_PS3 || PE_PLAT_IS_PSVITA
#include "PrimeEngine/APIAbstraction/PS3/PS3_PadInput/PS3_PadInput.h"
#endif

#if APIABSTRACTION_D3D9 || APIABSTRACTION_D3D11 || APIABSTRACTION_GLPC
#include "../DirectX9/DX9_KeyboardMouse/DX9_KeyboardMouse.h"
#endif//API abstraction

// Sibling/Children includes

namespace PE {
namespace Components {
// This struct polls for controller input and fires off events accordingly
struct Input
    : public Component
{
	PE_DECLARE_CLASS(Input);

	Input(PE::GameContext &context, PE::MemoryArena arena, PE::Handle hMyself);
	static void Construct(PE::GameContext &context, PE::MemoryArena arena);
	static Input *Instance(){return s_hMyself.getObject<Input>();}

	// Component ------------------------------------------------------------
	virtual void addDefaultComponents();

	// Individual events -------------------------------------------------------
	static PE::Handle s_hMyself;

};
}; // namespace Components
}; // namespace PE


#endif//File guard