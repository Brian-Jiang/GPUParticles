#ifndef __PYENGINE_2_0_LIGHT_H__
#define __PYENGINE_2_0_LIGHT_H__

// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

// Outer-Engine includes
#include <assert.h>

// Inter-Engine includes
#include "PrimeEngine/MemoryManagement/Handle.h"
#include "PrimeEngine/PrimitiveTypes/PrimitiveTypes.h"
#include "PrimeEngine/Events/Component.h"
#include "PrimeEngine/Utils/Array/Array.h"
#include "PrimeEngine/Math/Vector4.h"
#include "PrimeEngine/Math/Vector3.h"

#include "PrimeEngine/Render/ShaderActions/SetPerObjectGroupConstantsShaderAction.h"
// Sibling/Children includes

namespace PE {
namespace Components {

struct Light : public Component
{

	PrimitiveTypes::Bool isTheShadowCaster;
	PE_DECLARE_CLASS(Light);

	// Constructor -------------------------------------------------------------
	Light(
		PE::GameContext &context,
		PE::MemoryArena arena,
		Handle hMyself,
		Vector3 pos,
		Vector3 xdir,
		Vector3 ydir,
		Vector3 zdir,
		Vector4 ambient,
		Vector4 diffuse,
		Vector4 specular,
		Vector3 att,
		PrimitiveTypes::Float32 spotPow,
		PrimitiveTypes::Float32 range,
		PrimitiveTypes::Bool isShadowCaster,
		PrimitiveTypes::Int32 type) : Component(context, arena, hMyself)
	{
		memset(&m_cbuffer, 0, sizeof(m_cbuffer));
		m_base.setPos(pos);
		m_cbuffer.pos = pos;
		m_cbuffer.dir = zdir;
		m_base.setU(xdir);
		m_base.setV(ydir);
		m_base.setN(zdir);
		m_cbuffer.ambient = ambient;
		m_cbuffer.diffuse = diffuse;
		m_cbuffer.spec = specular;
		m_cbuffer.att = att;
		m_cbuffer.spotPower = spotPow;
		m_cbuffer.range = range;
		isTheShadowCaster = isShadowCaster;
		m_cbuffer.type = (PrimitiveTypes::Float32)(type);
	}

	virtual ~Light(){}

	// Component ------------------------------------------------------------

	virtual void addDefaultComponents() ;

	PE_DECLARE_IMPLEMENT_EVENT_HANDLER_WRAPPER(do_CALCULATE_TRANSFORMATIONS);
	virtual void do_CALCULATE_TRANSFORMATIONS(Events::Event *pEvt);
	virtual PrimitiveTypes::Bool castsShadow(); //Determines if the light has the shadow caster boolean = true
	virtual void setCastsShadow(PrimitiveTypes::Bool b); //Set this light to cast a shadow, make all other lights not cast shadows

	// Individual events -------------------------------------------------------

	Handle m_hParent;
	Matrix4x4 m_base;
	Matrix4x4 m_worldTransform; // is calculated before every draw via Events::CALULCATE_TRANSFORMATIONS
	Matrix4x4 m_worldToViewTransform; // objects in world space are multiplied by this to get them into camera's coordinate system (view space)
	Matrix4x4 m_viewToProjectedTransform; // objects in local (view) space are multiplied by this to get them to screen space

	SetPerObjectGroupConstantsShaderAction::hlsl_Light m_cbuffer; // gpu mirror : values that will be set into gpu registers vi shader action hlsl_cbPerObjectGroup_c1
};

}; // namespace Components
}; // namespace PE

#endif
