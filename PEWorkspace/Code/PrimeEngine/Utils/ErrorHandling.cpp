
// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

#include "ErrorHandling.h"

// Outer-Engine includes
#include <assert.h>

#if APIABSTRACTION_D3D9 || APIABSTRACTION_D3D11 || (APIABSTRACTION_OGL && APIABSTRACTION_GLPC)
#define _WINSOCKAPI_   /* Prevent inclusion of winsock.h in windows.h */
#include <windows.h>
#endif
#include <stdio.h>
#include <stdarg.h>

// returns true if user chooses to retry
bool PEASSERT(bool expr, const char *format, ...)
{ 

	if (!(expr))
	{
		char buf[256];
		//char newFrmt[256];
		//sprintf(newFrmt, "%s%s", format, "\nAbort Execution?");
		va_list ap;
		// You will get an unused variable message here -- ignore it.
		va_start(ap, format);
#if APIABSTRACTION_PS3 || APIABSTRACTION_IOS || PE_PLAT_IS_PSVITA
		vsprintf(buf, format, ap);
#else
		vsprintf_s<256>(buf, format, ap);
#endif
		va_end(ap);
		assert(false);
#if !APIABSTRACTION_IOS && !APIABSTRACTION_PS3 && !PE_PLAT_IS_PSVITA
		switch (MessageBoxA(0, buf, "PyEngine Error", MB_ABORTRETRYIGNORE))
		{
		case IDABORT:
			assert(!"PEASSERT Triggered Abort");
			break;
		case IDRETRY:
			return true;
			break;
		case IDIGNORE:
			return false;
			break;
		};

#endif	
	}
	return false; // succeeded no need to retry

} 

void
PEERROR(	const char *format, ...)
{
	char buf[256];
	va_list ap;
	// You will get an unused variable message here -- ignore it.
	va_start(ap, format);
#if APIABSTRACTION_PS3 || APIABSTRACTION_IOS || PE_PLAT_IS_PSVITA
	vsprintf(buf, format, ap);
#else
	vsprintf_s<256>(buf, format, ap);
#endif
	va_end(ap);
#if !APIABSTRACTION_PS3 && !APIABSTRACTION_IOS && !PE_PLAT_IS_PSVITA
	switch (MessageBoxA(0, buf, "PyEngine Error", MB_ABORTRETRYIGNORE))
	{
	case IDABORT:
		exit(-1);
	};
#endif
}


void
_PEPRINT(	const char *format, ...)
{
	static char buf[32000];
	va_list ap;
	// You will get an unused variable message here -- ignore it.
	va_start(ap, format);
#if APIABSTRACTION_PS3 || APIABSTRACTION_IOS || PE_PLAT_IS_PSVITA
	vsprintf(buf, format, ap);
	printf(buf);
#else
	vsprintf_s<32000>(buf, format, ap);
	OutputDebugStringA(buf);
#endif
	va_end(ap);
}


