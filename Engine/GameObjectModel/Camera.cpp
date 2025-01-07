
#include "Camera.h"

#include "../Lua/LuaEnvironment.h"
namespace PE {
namespace Components{

PE_IMPLEMENT_CLASS1(Camera, Component);

Camera::Camera(PE::GameContext &context, PE::MemoryArena arena, Handle hMyself, Handle hParentSpaceSceneNode) : Component(context, arena, hMyself)
{
	m_hCameraSceneNode = Handle("CAMERA_SCENE_NODE", sizeof(CameraSceneNode));
	CameraSceneNode *pcam = new(m_hCameraSceneNode) CameraSceneNode(context, arena, m_hCameraSceneNode);
	pcam->addDefaultComponents();
	hParentSpaceSceneNode.getObject<SceneNode>()->addComponent(m_hCameraSceneNode);
}

}; // namespace Components
}; // namespace PE
