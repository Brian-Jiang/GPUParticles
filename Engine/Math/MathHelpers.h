#ifndef __PYENGINE_2_0_MATH_HELPERS_H__
#define __PYENGINE_2_0_MATH_HELPERS_H__

// APIAbstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

// Outer-Engine includes
// Windows
#if APIABSTRACTION_D3D9 | APIABSTRACTION_D3D11 | defined(APIABSTRACTION_OGL)
#define _USE_MATH_DEFINES
#include <math.h>
#endif

// Inter-Engine includes
#include "PrimeEngine/MemoryManagement/Handle.h"
#include "PrimeEngine/PrimitiveTypes/PrimitiveTypes.h"
//#include "PrimeEngine/Math/Vector3.h"
//#include "PrimeEngine/Math/Box.h"

// Sibling/Children includes

// #define's
#if APIABSTRACTION_D3D9 || APIABSTRACTION_D3D11 || APIABSTRACTION_OGL || PE_PLAT_IS_PSVITA
	// D3D uses Left-Handed Coordinate System
	#define LEFT_COORDINATE_SYSTEM
#endif

inline bool compareFloats(const PrimitiveTypes::Float32 &f0, const PrimitiveTypes::Float32 &f1)
{
	return (fabs(f0 - f1) < PrimitiveTypes::Constants::c_Epsilon_F32);
}

//inline bool checkBoxPlaneOnSameSide(const Box& box, const Vector3& point, const Vector3& normal) {
//	//auto& center = box.m_center;
//	//auto& extent = box.m_extent;
//	//Vector3 near_topLeft(center.m_x - extent.m_x, center.m_y + extent.m_y, center.m_z - extent.m_z);
// //   Vector3 near_topRight(center.m_x + extent.m_x, center.m_y + extent.m_y, center.m_z - extent.m_z);
// //   Vector3 near_bottomLeft(center.m_x - extent.m_x, center.m_y - extent.m_y, center.m_z - extent.m_z);
// //   Vector3 near_bottomRight(center.m_x + extent.m_x, center.m_y - extent.m_y, center.m_z - extent.m_z);
// //   Vector3 far_topLeft(center.m_x - extent.m_x, center.m_y + extent.m_y, center.m_z + extent.m_z);
// //   Vector3 far_topRight(center.m_x + extent.m_x, center.m_y + extent.m_y, center.m_z + extent.m_z);
// //   Vector3 far_bottomLeft(center.m_x - extent.m_x, center.m_y - extent.m_y, center.m_z + extent.m_z);
// //   Vector3 far_bottomRight(center.m_x + extent.m_x, center.m_y - extent.m_y, center.m_z + extent.m_z);
// //   if ((near_topLeft - point).dotProduct(normal) > 0) return false;
// //   if ((near_topRight - point).dotProduct(normal) > 0) return false;
// //   if ((near_bottomLeft - point).dotProduct(normal) > 0) return false;
// //   if ((near_bottomRight - point).dotProduct(normal) > 0) return false;
// //   if ((far_topLeft - point).dotProduct(normal) > 0) return false;
// //   if ((far_topRight - point).dotProduct(normal) > 0) return false;
// //   if ((far_bottomLeft - point).dotProduct(normal) > 0) return false;
// //   if ((far_bottomRight - point).dotProduct(normal) > 0) return false;
//
//    return true;
//}

template <typename T>
T max(const T& a, const T& b)
{
	return a > b ? a : b;
}

template <typename T>
T min(const T& a, const T& b)
{
	return a < b ? a : b;
}
namespace pemath
{
	PE_INLINE float sign(float f) {
#if APIABSTRACTION_PSVITA || APIABSTRACTION_IOS || PE_PLAT_IS_PS4
		return (float)copysign(1.0, f);
#else
		return (float)_copysign(1.0, f);
#endif
	}
	PE_INLINE float clamp(float minv, float maxv, float v) { return max(min(maxv, v), minv); }
}


#endif
