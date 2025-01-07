#ifndef __PYENGINE_2_0_LINEMESH_H__
#define __PYENGINE_2_0_LINEMESH_H__

#define NOMINMAX
// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

// Outer-Engine includes
#include <assert.h>

// Inter-Engine includes
#include "PrimeEngine/APIAbstraction/Effect/Effect.h"

// Sibling/Children includes
#include "Mesh.h"

namespace PE {
namespace Components {

struct LineMesh : public Mesh
{
	PE_DECLARE_CLASS(LineMesh);

	// Constructor -------------------------------------------------------------
	LineMesh(PE::GameContext &context, PE::MemoryArena arena, Handle hMyself) : Mesh(context, arena, hMyself)
	{
		m_loaded = false;
	}

	virtual ~LineMesh(){}

	virtual void addDefaultComponents();

	PE_DECLARE_IMPLEMENT_EVENT_HANDLER_WRAPPER(do_GATHER_DRAWCALLS);
	virtual void do_GATHER_DRAWCALLS(Events::Event *pEvt);

	void loadFrom3DPoints_needsRC(float *vals, int numPoints, const char *techName, int &threadOwnershipMask);

	PrimitiveTypes::Bool m_loaded;
	Handle m_meshCPU;
};

}; // namespace Components
}; // namespace PE
#endif
