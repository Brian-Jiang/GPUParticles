#define NOMINMAX
#include "CameraManager.h"
#include "../Lua/LuaEnvironment.h"
namespace PE {
namespace Components{

PE_IMPLEMENT_CLASS1(CameraManager, Component);

Handle CameraManager::s_hInstance;

CameraManager::CameraManager(PE::GameContext &context, PE::MemoryArena arena, Handle hMyself) : Component(context, arena, hMyself)
{
	Handle hDebugCamera("CAMERA", sizeof(Camera));
	Camera *debugCamera = new(hDebugCamera) Camera(context, arena, hDebugCamera, RootSceneNode::InstanceHandle());
	debugCamera->addDefaultComponents();

	setCamera(DEBUG_CAM, hDebugCamera);
	selectActiveCamera(DEBUG_CAM);

	SceneNode *pCamSN = debugCamera->getCamSceneNode();
	
	pCamSN->m_base.setPos(Vector3(0.0f,0.0f,-5.0f));
	pCamSN->m_base.setPos(Vector3(-5.0f,4.0f,-7.0f));
	pCamSN->m_base.turnInDirection(Vector3(0.0f, 1.0f, -1.1f) - Vector3(-5.0f,4.0f,-7.0f));
}

void CameraManager::Construct(PE::GameContext &context, PE::MemoryArena arena)
{
	Handle h("CAMERA_MANAGER", sizeof(CameraManager));
	CameraManager *pCameraManager = new(h) CameraManager(context, arena, h);
	pCameraManager->addDefaultComponents();
	SetInstance(h);
}
	
void CameraManager::SetInstance(Handle h){s_hInstance = h;}
CameraManager *CameraManager::Instance(){return s_hInstance.getObject<CameraManager>();}

bool CameraManager::selectActiveCamera(CameraType type)
{
	m_activeCameraType = type;

	switch(type){
		case PLAYER:
			if(m_hPlayerCamera.isValid())
			{
				m_hActiveCamera = m_hPlayerCamera;
			}
			else //In case level file has no fpplayer
			{
				m_hActiveCamera = m_hDebugCamera;
				return false;
			}
			break;
		case DEBUG_CAM:
			m_hActiveCamera = m_hDebugCamera;
			break;
		case CINEMATIC:
			m_hActiveCamera = m_hCinematicCamera;
			break;
		case VEHICLE:
			m_hActiveCamera = m_hVehicleCamera;
			break;
        case CameraType_Count:
            assert(false);
            break;
	};
	return true;
}
	
Camera *CameraManager::getCamera(CameraType type)
{
	switch(type){
		case PLAYER:
			return m_hPlayerCamera.getObject<Camera>();
			break;
		case DEBUG_CAM:
			return m_hDebugCamera.getObject<Camera>();
			break;
		case CINEMATIC:
			return m_hCinematicCamera.getObject<Camera>();
			break;
		case VEHICLE:
			return m_hVehicleCamera.getObject<Camera>();
			break;
		default:
			return m_hPlayerCamera.getObject<Camera>();
	};
}	

Handle CameraManager::getCameraHandle(CameraType type)
{
	switch(type){
		case PLAYER:
			return m_hPlayerCamera;
			break;
		case DEBUG_CAM:
			return m_hDebugCamera;
			break;
		case CINEMATIC:
			return m_hCinematicCamera;
			break;
		case VEHICLE:
			return m_hVehicleCamera;
			break;
		default:
			return m_hDebugCamera;
	};
}

void CameraManager::setCamera(CameraType type, Handle h)
{
	switch(type){
		case PLAYER:
			m_hPlayerCamera = h;
			break;
		case DEBUG_CAM:
			m_hDebugCamera = h;
			break;
		case CINEMATIC:
			m_hCinematicCamera = h;
			break;
		case VEHICLE:
			m_hVehicleCamera = h;
			break;
	};
}

Camera *CameraManager::getActiveCamera()
{
	return getActiveCameraHandle().getObject<Camera>();
}

CameraManager::CameraType CameraManager::getActiveCameraType()
{
	return m_activeCameraType;
}
	
Handle CameraManager::getActiveCameraHandle()
{
	if (m_hActiveCamera.isValid())
		return m_hActiveCamera;
	// TODO: output warning
	if (m_hDebugCamera.isValid())
		return m_hDebugCamera;
	return Handle();
}

}; // namespace Components
}; // namespace PE
