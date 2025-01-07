
#define NOMINMAX

// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

// Outer-Engine includes

// Inter-Engine includes

// Sibling/Children includes
#include "WinApplication.h"

namespace PE {
WinApplication *WinApplication::s_pWindowsEventHandler = NULL;

WinApplication::WinApplication(PE::GameContext &context, unsigned int width, unsigned int height, const char *caption)
{
	s_pWindowsEventHandler = this;

	MainFunctionArgs *pArgs = context.getMainFunctionArgs();

	HINSTANCE hInstance = pArgs->m_hInstance;

	// Windows API
	WNDCLASS wc;
	wc.style         = CS_OWNDC;

	wc.lpfnWndProc   = WinApplication::s_windowsEventHandler; 
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = hInstance;
	wc.hIcon         = LoadIcon(0, IDI_APPLICATION);
	wc.hCursor       = LoadCursor(0, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
	wc.lpszMenuName  = 0;

	wc.lpszClassName = L"D3DWndClassName_PrimeEngine";

	if( !RegisterClass(&wc) )
	{
		MessageBoxW(0, L"RegisterClass FAILED", 0, 0);
		PostQuitMessage(0);
	}

	// Compute window rectangle dimensions based on requested client area dimensions.
	RECT R = { 0, 0, width, height };
	AdjustWindowRect(&R, WS_OVERLAPPEDWINDOW, false);
	int winWindowWidth  = R.right - R.left;
	int winWindowHeight = R.bottom - R.top;

	m_windowHandle = CreateWindowA("D3DWndClassName_PrimeEngine", caption, 
		WS_OVERLAPPEDWINDOW|WS_SYSMENU|WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, winWindowWidth, winWindowHeight, 0, 0, hInstance, this); 
	if( !m_windowHandle )
	{
		MessageBoxW(0, L"CreateWindow FAILED", 0, 0);
		PostQuitMessage(0);
	}

	//ShowWindow(m_windowHandle, SW_SHOW);
	//UpdateWindow(m_windowHandle);
}
	
void WinApplication::exit()
{
	// This will send WM_QUIT
	PostQuitMessage(0);
}
LRESULT CALLBACK WinApplication::windowsEventHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
		// Is sent when window is closed (x is clicked or Alt+F4)
		case WM_DESTROY:
			{
				// This will send WM_QUIT
				PostQuitMessage(0);
				// need to post a message to global event queue
				Handle h("EVENT", sizeof(Events::Event_CLOSED_WINDOW));
				new (h) Events::Event_CLOSED_WINDOW;
		
				Events::EventQueueManager::Instance()->add(h, Events::QT_GENERAL);
				return 0;
			}
		default:
			// Windows default handler
			return DefWindowProc(hWnd, msg, wParam, lParam);
	};
	// In case we need some special windows event handlers, uncomment this code

	/*
	switch( msg )
	{
		// WM_ACTIVATE is sent when the window is activated or deactivated.  
		// We pause the game when the window is deactivated and unpause it 
		// when it becomes active.  
	case WM_ACTIVATE:
		if( LOWORD(wParam) == WA_INACTIVE )
		{
			//Paused = true;
		}
		else
		{
			//Paused = false;
		}
	// WM_SIZE is sent when the user resizes the window.  
	case WM_SIZE:
	{
		// Save the new client area dimensions.
		int mClientWidth  = LOWORD(lParam);
		int mClientHeight = HIWORD(lParam);
		if( wParam == SIZE_MINIMIZED )
		{
		}
		else if( wParam == SIZE_MAXIMIZED )
		{
		}
		else if( wParam == SIZE_RESTORED )
		{
			// Restoring from minimized state?
			//if( mMinimized )
			//{
			//}
			//Restoring from maximized state?
			//else if( mMaximized )
			//{
			//}
			//else if( mResizing )
			//{
				// If user is dragging the resize bars, we do not resize 
				// the buffers here because as the user continuously 
				// drags the resize bars, a stream of WM_SIZE messages are
				// sent to the window, and it would be pointless (and slow)
				// to resize for each WM_SIZE message received from dragging
				// the resize bars.  So instead, we reset after the user is 
				// done resizing the window and releases the resize bars, which 
				// sends a WM_EXITSIZEMOVE message.
			//}
			//else // API call such as SetWindowPos or mSwapChain->SetFullscreenState.
			//{
			//}
		//	}
		}
	}//case WM_SIZE:
	
	// WM_EXITSIZEMOVE is sent when the user grabs the resize bars.
	case WM_ENTERSIZEMOVE:
	// WM_EXITSIZEMOVE is sent when the user releases the resize bars.
	// Here we reset everything based on the new window dimensions.
	case WM_EXITSIZEMOVE:
	// WM_DESTROY is sent when the window is being destroyed.
	case WM_DESTROY:
		//PostQuitMessage(0);
	// The WM_MENUCHAR message is sent when a menu is active and the user presses 
	// a key that does not correspond to any mnemonic or accelerator key. 
	case WM_MENUCHAR:
		// Don't beep when we alt-enter.
		//return MAKELRESULT(0, MNC_CLOSE);
	// Catch this message so to prevent the window from becoming too small.
	case WM_GETMINMAXINFO:
		//((MINMAXINFO*)lParam)->ptMinTrackSize.x = 200;
		//((MINMAXINFO*)lParam)->ptMinTrackSize.y = 200; 
		//return 0;
	default: break;
	};

	return DefWindowProc(m_windowHandle, msg, wParam, lParam);
	*/
}

Application* Application::Construct(PE::GameContext &context, unsigned int width, unsigned int height, const char *caption)
{
	Handle h("WinApplication", sizeof(WinApplication));
	WinApplication *pApp = new (h) WinApplication(context, width, height, caption);
	context.m_pApplication = pApp;
	return pApp;
}

}; // namespace PE
