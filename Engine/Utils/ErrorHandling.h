#ifndef __PYENGINE_2_0_ERROR_HANDLING_H__
#define __PYENGINE_2_0_ERROR_HANDLING_H__

// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

// Outer-Engine includes
#include <assert.h>
#ifdef _DEBUG
	#define PEDebugAssert(...) PEASSERT(__VA_ARGS__)
#else
	#define PEDebugAssert(...)
#endif


// returns true if user chooses to retry
bool PEASSERT(bool expr, const char *format, ...);


void
PEERROR(	const char *format, ...);

void _PEPRINT(const char *format, ...);

#define PEINFO(format, ...) _PEPRINT("PE: Info: " format "\n", ##__VA_ARGS__)
#define PEINFOSTR(str) PEINFO("%s", str)
#define PEWARN(format, ...) _PEPRINT("PE: Warning: " format "\n", ##__VA_ARGS__)
#endif



