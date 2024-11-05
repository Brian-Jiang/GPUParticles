#ifndef __PYENGINE_2_0_CAMERA_MANAGER__
#define __PYENGINE_2_0_CAMERA_MANAGER__

#define NOMINMAX
// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

// Outer-Engine includes

// Inter-Engine includes
#include "../Utils/Array/Array.h"
#include "../Utils/PEString.h"

// Sibling/Children includes
#include "../GameObjectModel/Camera.h"
#include "RootSceneNode.h"
namespace PE {
namespace Components {


struct CameraManager : public Component
{
	PE_DECLARE_CLASS(CameraManager);

	enum CameraType{
		PLAYER = 0,
		DEBUG_CAM = 1,
		CINEMATIC = 2,
		VEHICLE = 3,
		CameraType_Count
	};

	CameraManager(PE::GameContext &context, PE::MemoryArena arena, PE::Handle hMyself);
	virtual ~CameraManager(){}

	static void Construct(PE::GameContext &context, PE::MemoryArena arena);
	static void SetInstance(Handle h);
	bool selectActiveCamera(CameraType type);
	Camera *getCamera(CameraType type);
	Handle getCameraHandle(CameraType type);
	void setCamera(CameraType type, Handle h);
	Camera *getActiveCamera();
	CameraType getActiveCameraType();
	Handle getActiveCameraHandle();

	static CameraManager *Instance();

	static Handle s_hInstance;

	//Cameras
	Handle m_hActiveCamera;
	CameraType m_activeCameraType;
	Handle m_hPlayerCamera;
	Handle m_hDebugCamera;
	Handle m_hCinematicCamera;
	Handle m_hVehicleCamera;
};

}; // namespace Components
}; // namespace PE
#endif
