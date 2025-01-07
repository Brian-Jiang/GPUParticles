#ifndef __PYENGINE_2_0_WinTimer_H___
#define __PYENGINE_2_0_WinTimer_H___

// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"
#if APIABSTRACTION_D3D9 || APIABSTRACTION_D3D11 || APIABSTRACTION_GLPC

// Outer-Engine includes
#ifdef _WIN32
#define _WINSOCKAPI_   /* Prevent inclusion of winsock.h in windows.h */
#include <windows.h>
#endif

// Inter-Engine includes
#include "PrimeEngine/MemoryManagement/Handle.h"
#include "PrimeEngine/Utils/PEClassDecl.h"

// Sibling/Children includes

// This class contains timer functionality provided by Windows OS and DirectX 10
class WinTimer : public PE::PEAllocatableAndDefragmentable
{
public:
	// returns processor ticks
	static __int64 GetCurrentTicks()
	{
		__int64 currTime;
		QueryPerformanceCounter((LARGE_INTEGER*)(&currTime));
		return currTime;
	}

	static double TimeDifferenceInSeconds(__int64 ticks0, __int64 ticks1)
	{
		return (double)(ticks1 - ticks0) * s_dSecondsPerTick;
	}

	static double TimeInSeconds(__int64 ticks0)
	{
		return (double)(ticks0) * s_dSecondsPerTick;
	}

	static void Initialize()
	{
		__int64 freq;
		QueryPerformanceFrequency((LARGE_INTEGER*)&freq);

		s_dSecondsPerTick = 1.0 / (double)freq;
	}

public:
	// Member vars
	static double s_dSecondsPerTick;
};

#endif // API Abstraction
#endif // file guard
