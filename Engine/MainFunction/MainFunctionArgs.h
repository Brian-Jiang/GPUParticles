#ifndef __PYENGINE_2_0_MAINFUNCTIONARGS_H___
#define __PYENGINE_2_0_MAINFUNCTIONARGS_H___

// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"



// Inter-Engine includes
#if APIABSTRACTION_D3D9 | APIABSTRACTION_D3D11 | APIABSTRACTION_OGL 

// Outer-Engine includes
#ifdef _WIN32
#define _WINSOCKAPI_   /* Prevent inclusion of winsock.h in windows.h */
#include <windows.h>
#endif

#endif

#include "PrimeEngine/MemoryManagement/Handle.h"
#include "PrimeEngine/Utils/PEClassDecl.h"
#include "PrimeEngine/Utils/Array/Array.h"
#include "PrimeEngine/Utils/PEString.h"

struct MainFunctionArgs : public PE::PEAllocatableAndDefragmentable
{
#	if PE_PLAT_IS_WIN32
		MainFunctionArgs(PE::GameContext &context, PE::MemoryArena arena, const char *lpCmdLine, HINSTANCE hInstance);
#	else
		MainFunctionArgs(PE::GameContext &context, PE::MemoryArena arena, const char *lpCmdLine);
#endif

	const char *gameProjRoot();
	PrimitiveTypes::Bool hasArgs();

	const char *getArg(PrimitiveTypes::UInt32 i);

	// Member Variables---------------------------------------------------------

    void setArgs(int argc, char const **argv);
    
#if PE_PLAT_IS_WIN32
    HINSTANCE m_hInstance;
#endif
    
	PE::GameContext *m_pContext;
	PE::MemoryArena m_arena;

	char m_gameProjRoot[256]; // path to engine root
	Array<PEString> m_args;
	
};

#endif
