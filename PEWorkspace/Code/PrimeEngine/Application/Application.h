#ifndef __pe_application_h__
#define __pe_application_h__

#define NOMINMAX
// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

// Inter-Engine includes
#include "PrimeEngine/MainFunction/MainFunctionArgs.h"
#include "PrimeEngine/MemoryManagement/Handle.h"

// Sibling/Children includes

// This class is implementation of IRenderer for D3D10

namespace PE {

// forward declaration of texture class
struct TextureGPU;

class Application : public PE::PEAllocatableAndDefragmentable
{
public:

	// platform specific implementations should implement this
	static Application* Construct(PE::GameContext &context, unsigned int width, unsigned int height, const char *caption);

	virtual void processOSEventsIntoGlobalEventQueue() = 0;
	virtual void exit() = 0;
public:
	

};
}; // namespace PE

#endif // File guard
