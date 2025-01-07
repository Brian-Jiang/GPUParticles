#ifndef __PYENGINE_2_0_TIMER_H__
#define __PYENGINE_2_0_TIMER_H__

// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

// Outer-Engine includes
#ifdef _WIN32

#define _WINSOCKAPI_   /* Prevent inclusion of winsock.h in windows.h */
#include <windows.h>

#endif

// Inter-Engine includes
#if APIABSTRACTION_D3D9 || APIABSTRACTION_D3D11 || APIABSTRACTION_GLPC
#include "WinTimer.h"
#elif APIABSTRACTION_OGL
#include "PrimeEngine/Render/GLRenderer.h"
#elif PE_PLAT_IS_PSVITA

#if APIABSTRACTION_PS3

#endif
#endif

#include "PrimeEngine/MemoryManagement/Handle.h"

// Sibling/Children includes

class Timer : public PE::PEAllocatableAndDefragmentable
{
public:
	Timer()
#if !APIABSTRACTION_PS3 && !APIABSTRACTION_IOS && !PE_PLAT_IS_PSVITA
		: m_now(0)
		, m_before(0)
#endif
	{
#if PE_PLAT_IS_PSVITA
		m_before.quad = 0;
		m_now.quad = 0;
#endif
		Tick();
	}

	static void Initialize();

	void Tick()
	{
		#if APIABSTRACTION_D3D9 || APIABSTRACTION_D3D11 || APIABSTRACTION_GLPC
			m_before = m_now;
			m_now = WinTimer::GetCurrentTicks();

			//// note to measure gpu time, use glQueryCounter(queryID[0], GL_TIMESTAMP);
			// http://www.lighthouse3d.com/cg-topics/opengl-timer-query/
		#elif APIABSTRACTION_PS3
			m_before = m_now;
			m_now = sys_time_get_system_time();
		#elif APIABSTRACTION_IOS
            m_before = m_now;
            m_now = CFAbsoluteTimeGetCurrent();
		#elif PE_PLAT_IS_PSVITA
			
		#elif PE_PLAT_IS_PS4
			

        #endif
	}

	float GetTimeDeltaInSeconds()
	{
		#if APIABSTRACTION_D3D9 || APIABSTRACTION_D3D11 || APIABSTRACTION_GLPC
			return (float)(WinTimer::TimeDifferenceInSeconds(m_before, m_now));
		#elif APIABSTRACTION_IOS
            return (float)(((double)(m_now - m_before)) * 1.0);
		#elif APIABSTRACTION_PS3
			return (float)(((double)(m_now - m_before)) * .000001);
		#elif PE_PLAT_IS_PSVITA
		#elif PE_PLAT_IS_PS4
		#endif
	}

	float TickAndGetTimeDeltaInSeconds()
	{
		Tick();
		return GetTimeDeltaInSeconds();
	}

	#if APIABSTRACTION_D3D9 || APIABSTRACTION_D3D11 || APIABSTRACTION_GLPC
	typedef __int64 TimeType;
	#elif APIABSTRACTION_PS3
	typedef system_time_t TimeType;
	#elif APIABSTRACTION_IOS
    typedef CFTimeInterval TimeType;
	#elif PE_PLAT_IS_PSVITA
	#elif PE_PLAT_IS_PS4
	#endif

	TimeType GetTime()
	{
		return m_now;
	}

	TimeType TickAndGetCurrentTime()
	{
		Tick();
		return GetTime();
	}

	static float GetTimeDeltaInSeconds(TimeType t0, TimeType t1)
	{
#if APIABSTRACTION_D3D9 || APIABSTRACTION_D3D11 || APIABSTRACTION_GLPC
		return (float)(WinTimer::TimeDifferenceInSeconds(t0, t1));
#elif APIABSTRACTION_IOS
        return (float)(((double)(t1 - t0)) * 1.0);
#elif PE_PLAT_IS_PS4
#elif PE_PLAT_IS_PSVITA
#endif
	}

public:
	
private:
	#if APIABSTRACTION_D3D9 || APIABSTRACTION_D3D11 || APIABSTRACTION_GLPC
		TimeType m_now;
		TimeType m_before;
	#elif APIABSTRACTION_PS3
		TimeType m_now;
		TimeType m_before;
	#elif APIABSTRACTION_IOS
        TimeType m_now;
        TimeType m_before;
	#elif PE_PLAT_IS_PSVITA
		TimeType m_now;
		TimeType m_before;
	#elif PE_PLAT_IS_PS4
	TimeType m_now;
	TimeType m_before;
#endif
};


#endif