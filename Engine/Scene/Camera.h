#ifndef __PYENGINE_2_0_CAMERA_H__
#define __PYENGINE_2_0_CAMERA_H__

// API Abstraction
#include "../APIAbstraction/APIAbstractionDefines.h"

// Outer-Engine includes
#include <assert.h>

// Inter-Engine includes
#include "../APIAbstraction/GPUScreen/IRenderer.h"
#include "../MemoryManagement/Handle.h"
#include "../PrimitiveTypes/PrimitiveTypes.h"
#include "../Events/EventHandler.h"
#include "../Events/CommonEvents.h"
#include "../Utils/Array/Array.h"
#include "../APIAbstraction/CameraOps/CameraOps.h"

// Sibling/Children includes

namespace Scene{

struct Camera : public Events::EventHandler
{
	// Custom memory management
	void *operator new(size_t size, Handle &h)
	{
		// create this object in allocated memory
		return h.getObject();
	}

	void operator delete(void *ptr, Handle &h)
	{
		// don't do anything
	}

	// Constructor -------------------------------------------------------------
	Camera()
	{
		
	}

	// EventHandler ------------------------------------------------------------

	virtual void handleEvent(Events::Event *pEvt)
	{
		switch (pEvt->m_type)
		{
			case Events::CALCULATE_TRANSFORMATIONS:
			{
				Events::CalculateTransformationsEvtData *pData = pEvt->getEventData<Events::CalculateTransformationsEvtData>();
				
				m_worldTransform = pData->m_parentTransform * m_base;

				Vector3 pos = Vector3(m_worldTransform.m[0][3], m_worldTransform.m[1][3], m_worldTransform.m[2][3]);
				Vector3 n = Vector3(m_worldTransform.m[0][2], m_worldTransform.m[1][2], m_worldTransform.m[2][2]);
				Vector3 target = pos + n;
				Vector3 up = Vector3(m_worldTransform.m[0][1], m_worldTransform.m[1][1], m_worldTransform.m[2][1]);

				m_worldToViewTransform = CameraOps::CreateViewMatrix(pos, target, up);

				m_viewToProjectedTransform = CameraOps::CreateProjectionMatrix(0.25f * PrimitiveTypes::Constants::c_Pi_F32, 
					(PrimitiveTypes::Float32)(D3D10_GPUScreen::Instance()->m_width) / (PrimitiveTypes::Float32)(D3D10_GPUScreen::Instance()->m_height),
					1.0f, 10000.0f);
				
				
				break;
			}

			case Events::ADDED_AS_CHILD:
			{
				do_ADDED_AS_CHILD(pEvt);
				break;
			}
		};
	}
	
	// Individual events -------------------------------------------------------

	void do_ADDED_AS_CHILD(Events::Event *pEvt)
	{
		Events::AddedAsChildEvtData *pData = pEvt->getEventData<Events::AddedAsChildEvtData>();
		m_hParent = pData->m_hParent;
	}

	Handle m_hParent;
	Matrix4x4 m_base;
	Matrix4x4 m_worldTransform; // is calculated before every draw via Events::CALULCATE_TRANSFORMATIONS
	Matrix4x4 m_worldToViewTransform; // objects in world space are multiplied by this to get them into camera's coordinate system (view space)
	Matrix4x4 m_viewToProjectedTransform; // objects in local (view) space are multiplied by this to get them to screen space
};
}; // namespace Events
#endif
