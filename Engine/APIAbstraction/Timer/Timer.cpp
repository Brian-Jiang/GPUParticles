
// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

// Outer-Engine includes
#ifdef _WIN32
#define _WINSOCKAPI_   /* Prevent inclusion of winsock.h in windows.h */
#include <windows.h>
#endif

// Inter-Engine includes

// Sibling/Children includes
#include "Timer.h"


// timer itself is not a singleton, but it uses a singleton class WinTimer
void Timer::Initialize()
{
	#if APIABSTRACTION_D3D9 || APIABSTRACTION_D3D11 || APIABSTRACTION_GLPC
		WinTimer::Initialize();
	#endif
}
