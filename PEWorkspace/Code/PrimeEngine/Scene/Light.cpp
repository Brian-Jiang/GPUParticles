#include "Light.h"
#include "SceneNode.h"
#include "../Lua/LuaEnvironment.h"
#include "PrimeEngine/Events/StandardEvents.h"
#include "PrimeEngine/Math/CameraOps.h"
namespace PE {
namespace Components {

PE_IMPLEMENT_CLASS1(Light, Component);

void Light::addDefaultComponents()
{
	Component::addDefaultComponents();

	PE_REGISTER_EVENT_HANDLER(Events::Event_CALCULATE_TRANSFORMATIONS, Light::do_CALCULATE_TRANSFORMATIONS);
}

void Light::do_CALCULATE_TRANSFORMATIONS(Events::Event *pEvt)
{
	m_worldTransform = m_base;

	Handle hp = getFirstParentByType<SceneNode>();
	if (hp.isValid())
	{	
		m_worldTransform = hp.getObject<PE::Components::SceneNode>()->m_base * m_worldTransform;
	}

	Vector3 pos = Vector3(m_worldTransform.m[0][3], m_worldTransform.m[1][3], m_worldTransform.m[2][3]);
	Vector3 n = Vector3(m_worldTransform.m[0][2], m_worldTransform.m[1][2], m_worldTransform.m[2][2]);
	Vector3 target = pos + n;
	Vector3 up = Vector3(m_worldTransform.m[0][1], m_worldTransform.m[1][1], m_worldTransform.m[2][1]);

	m_worldToViewTransform = CameraOps::CreateViewMatrix(pos, target, up);

	m_viewToProjectedTransform = CameraOps::CreateProjectionMatrix(0.25f * PrimitiveTypes::Constants::c_Pi_F32, 1.3f, 0.25f, 100.0f);

	// store in hlsl mirror:
	m_cbuffer.pos = pos;

}
PrimitiveTypes::Bool Light::castsShadow(){
	return isTheShadowCaster;
}
void Light::setCastsShadow(PrimitiveTypes::Bool b){
	isTheShadowCaster=b;
	if(b==true){
		//TODO: turn the current shadowcasting light into a non-shadow-caster. 
		//One option would just be moving it to the front of the list of m_lights
	}
}

}; // namespace Components
}; // namespace PE


