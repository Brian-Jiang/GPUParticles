// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

// Outer-Engine includes

// Inter-Engine includes
#include "PrimeEngine/Lua/LuaEnvironment.h"

// Sibling/Children includes
#include "Input.h"


namespace PE {
namespace Components {

// static vars
PE::Handle Input::s_hMyself;

PE_IMPLEMENT_CLASS1(Input, Component);

Input::Input(PE::GameContext &context, PE::MemoryArena arena, PE::Handle hMyself)
    : Component(context, arena, hMyself)
{
	
}

void Input::addDefaultComponents()
{  
	Component::addDefaultComponents();
    
#if APIABSTRACTION_D3D9 | APIABSTRACTION_D3D11
	Handle xin("DX9_XINPUT", sizeof(DX9_XInput));
	DX9_XInput *pxin = new(xin) DX9_XInput(*m_pContext, m_arena, xin);
	pxin->addDefaultComponents();
	addComponent(xin);
#elif PE_PLAT_IS_PS3 || PE_PLAT_IS_PSVITA
	Handle xin("PS3_PADINPUT", sizeof(PS3_PadInput));
	PS3_PadInput *pxin = new(xin) PS3_PadInput(*m_pContext, m_arena, xin);
	pxin->addDefaultComponents();
	addComponent(xin);
#endif

#if APIABSTRACTION_D3D9 | APIABSTRACTION_D3D11 | APIABSTRACTION_GLPC

	Handle kmin("DX9_XINPUT", sizeof(DX9_KeyboardMouse));
	DX9_KeyboardMouse *pkmin = new(kmin) DX9_KeyboardMouse(*m_pContext, m_arena, kmin);
	pkmin->addDefaultComponents();
	addComponent(kmin);
#endif
}

void Input::Construct(PE::GameContext &context, PE::MemoryArena arena)
{
	s_hMyself = PE::Handle("RAW_DATA", sizeof(Input));
	Input *p = new(s_hMyself) Input(context, arena, s_hMyself);
	p->addDefaultComponents();
}

}; // namesapce Components
}; // namespace PE
