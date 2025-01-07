#ifndef __PYENGINE_2_0_CAMERA__
#define __PYENGINE_2_0_CAMERA__

// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

// Outer-Engine includes
#include <assert.h>

// Inter-Engine includes
#include "../Scene/CameraSceneNode.h"

// Sibling/Children includes

namespace PE {
namespace Components {

struct Camera : public Component
{
	PE_DECLARE_CLASS(Camera);


	// Constructor -------------------------------------------------------------
	// when created in a scene node
	Camera(PE::GameContext &context, PE::MemoryArena arena, Handle hMyself, Handle hParentSpaceSceneNode);
	virtual ~Camera(){}

	CameraSceneNode *getCamSceneNode()
	{
		return m_hCameraSceneNode.getObject<CameraSceneNode>();
	}

	// Component ------------------------------------------------------------
	//virtual void handleEvent(Events::Event *pEvt);

	// Individual events -------------------------------------------------------
	Handle m_hCameraSceneNode;
};
}; // namespace Components
}; // namespace PE

#endif
