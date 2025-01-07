#ifndef __pe_winapplication_h__
#define __pe_winapplication_h__

#define NOMINMAX
// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

#if PE_PLAT_IS_WIN32

// Let VC++ know we are compiling for WinXP and up.  This let's us use
// API functions specific to WinXP (e.g., WM_INPUT API).
#ifndef _WIN32_WINNT
#define _WIN32_WINNT   0x0600 // Vista
#endif

// Outer-Engine includes

#define _WINSOCKAPI_   /* Prevent inclusion of winsock.h in windows.h */
#include <windows.h>



// Inter-Engine includes
#include "PrimeEngine/MainFunction/MainFunctionArgs.h"
#include "PrimeEngine/MemoryManagement/Handle.h"
#include "PrimeEngine/Events/EventQueueManager.h"
#include "PrimeEngine/Events/StandardEvents.h"
#include "Application.h"

// Sibling/Children includes

// This class is implementation of IRenderer for D3D10

namespace PE {

// forward declaration of texture class
struct TextureGPU;

class WinApplication : public Application
{
public:
	WinApplication(PE::GameContext &context, unsigned int width, unsigned int height, const char *caption);
	static LRESULT CALLBACK s_windowsEventHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		return s_pWindowsEventHandler->windowsEventHandler(hWnd, msg, wParam, lParam);
	}

	LRESULT CALLBACK windowsEventHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	
	
	virtual void processOSEventsIntoGlobalEventQueue()
	{
		MSG msg = {0};
	
		// If there are Window messages then process them.
		// May need to use if(PeekMessage( &msg, D3D9Renderer::Instance()->getWindowHandle(), 0, 0, PM_REMOVE )) for Win XP
		// instead of GetMessage(..)
		//if (GetMessage(&msg, NULL, 0, 0))
		while (PeekMessage( &msg, getWindowHandle(), 0, 0, PM_REMOVE ))
		{
			// decode message
			TranslateMessage( &msg );
			// This will call message handler in D3D9Renderer
			DispatchMessage( &msg );
		}
	}

	virtual void exit();

	// Accessors ---------------------------------------------------------------
	HWND getWindowHandle() const
	{
		return m_windowHandle;
	}

public:
	
	HWND m_windowHandle;
	static WinApplication *s_pWindowsEventHandler;
	
};

}; // namespace PE
#endif // APIAbstraction
#endif // File guard
