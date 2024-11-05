#ifndef __PYENGINE_2_0_THREADING_H___
#define __PYENGINE_2_0_THREADING_H___

// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

#include "PrimeEngine/Utils/ErrorHandling.h"

#if APIABSTRACTION_IOS
#include <pthread/pthread.h>
#endif

#if PE_PLAT_IS_PS4
#endif

// Outer-Engine includes
#if APIABSTRACTION_D3D9 || APIABSTRACTION_D3D11

#ifdef _WIN32
#define _WINSOCKAPI_   /* Prevent inclusion of winsock.h in windows.h */
#include <windows.h>
#endif
#elif PE_PLAT_IS_PSVITA
#include <kernel.h>
#endif

// Inter-Engine includes

namespace PE {
namespace Threading {
	enum ThreadOwnedContexts
	{
		GameContext = 1 << 0,
		RenderContext = 1 << 1,
	};

	typedef unsigned int ThreadId;
	struct Mutex
	{
		int memCheck;
#if APIABSTRACTION_IOS
		pthread_mutex_t m_osLock;
#elif PE_PLAT_IS_PS4
		
#elif  PE_PLAT_IS_PSVITA
		
#else
		CRITICAL_SECTION m_osLock;
#endif
		ThreadId m_threadId;
		Mutex()
		{
			memCheck = 0x12121212;
			PEINFO("memCheck addr: %x", &m_osLock);
#if  APIABSTRACTION_IOS
			pthread_mutex_init(&m_osLock, 0);
#elif PE_PLAT_IS_PS4
			
#elif PE_PLAT_IS_PSVITA
			
#else
			InitializeCriticalSection(&m_osLock);
#endif
		}

		~Mutex()
		{
#if  APIABSTRACTION_IOS
			pthread_mutex_destroy(&m_osLock);
#elif PE_PLAT_IS_PS4
			
#elif PE_PLAT_IS_PSVITA
			
#else
			DeleteCriticalSection(&m_osLock);
#endif
		}

		bool lock(ThreadId threadId = 0)
		{
			m_threadId = threadId;
#if  APIABSTRACTION_IOS
			return pthread_mutex_lock(&m_osLock) == 0;
#elif PE_PLAT_IS_PS4
			
#elif PE_PLAT_IS_PSVITA
			
#else
			EnterCriticalSection(&m_osLock);
			return true;
#endif
		}

		void unlock()
		{
#if APIABSTRACTION_IOS
			pthread_mutex_unlock(&m_osLock);
#elif PE_PLAT_IS_PS4
			
#elif PE_PLAT_IS_PSVITA
			
#else
			LeaveCriticalSection(&m_osLock);
#endif
		}
	};

	struct ConditionVariable
	{
#if  APIABSTRACTION_IOS
		pthread_cond_t m_osCV;
#elif PE_PLAT_IS_PS4
		
#elif PE_PLAT_IS_PSVITA
		
#else
	CONDITION_VARIABLE m_osCV;
#endif
		Mutex &m_associatedLock;
		ConditionVariable(Mutex &lock)
			: m_associatedLock(lock)
		{
#if APIABSTRACTION_IOS
			pthread_cond_init(&m_osCV, 0);
#elif PE_PLAT_IS_PS4
			
#elif PE_PLAT_IS_PSVITA
			
#else
			 InitializeConditionVariable(&m_osCV);
#endif
		}

		~ConditionVariable()
		{
#if APIABSTRACTION_IOS
			pthread_cond_destroy(&m_osCV);
#elif PE_PLAT_IS_PS4
			
#elif PE_PLAT_IS_PSVITA
			
#else
			// I dont think there is a function for this?
			// DeleteConditionVariable(&m_osCV);
#endif
		}

		bool sleep()
		{
			
#if  APIABSTRACTION_IOS
			return pthread_cond_wait(&m_osCV, &m_associatedLock.m_osLock) == 0;
#elif PE_PLAT_IS_PS4
			
#elif PE_PLAT_IS_PSVITA
			
#else
			return SleepConditionVariableCS(&m_osCV, &m_associatedLock.m_osLock, INFINITE) == TRUE;
#endif
		}

		void signal()
		{
#if APIABSTRACTION_IOS
			pthread_cond_signal(&m_osCV);
#elif PE_PLAT_IS_PS4
			
#elif PE_PLAT_IS_PSVITA
			
#else
			
			WakeConditionVariable(&m_osCV);
			
#endif
		}
	};
#if APIABSTRACTION_IOS
	typedef pthread_t PEOsThread;
#elif PE_PLAT_IS_PSVITA
	
#elif PE_PLAT_IS_PS4
	
#else
	typedef HANDLE PEOsThread;
#endif

	 typedef void (*ThreadFunction)(void *params);


	struct PEThread
	{
		int m_start;
		PEOsThread m_osThread;
		ThreadFunction m_function;
		void *m_pParams;
		const static int PE_THREAD_STACK_SIZE = 1024 * 1024;
		// wrapper to call m_function
#if APIABSTRACTION_IOS
		static void *OSThreadFunction(void *params)
        {
#elif PE_PLAT_IS_PSVITA
		
#else
		static unsigned int __stdcall OSThreadFunction(void *params)
		{

#endif
			PEThread *pThread = static_cast<PEThread *>(params);
			PEASSERT(pThread->m_start == 0xdeadbeef, "bad ptr");
			(*pThread->m_function)(pThread->m_pParams);
			return 0;
		}

		void run()
		{
			m_start = 0xdeadbeef;
#if APIABSTRACTION_PS3 || APIABSTRACTION_IOS
			pthread_create(&m_osThread, NULL, PEThread::OSThreadFunction, this);
#elif PE_PLAT_IS_PS4
			
#elif PE_PLAT_IS_PSVITA
			
#else
			unsigned int threadId;
			m_osThread =(HANDLE)_beginthreadex(NULL,
				0,
				PEThread::OSThreadFunction,
				this,
				CREATE_SUSPENDED,
				&threadId);
			ResumeThread(m_osThread);
#endif
		}
	};
}; // Threading
}; // PE

#endif
