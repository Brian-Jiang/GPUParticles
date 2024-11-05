#ifndef __PYENGINE_2_0_VECTOR4_H__
#define __PYENGINE_2_0_VECTOR4_H__

// APIAbstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

// Outer-Engine includes

// Inter-Engine includes
#include "PrimeEngine/MemoryManagement/Handle.h"
#include "PrimeEngine/PrimitiveTypes/PrimitiveTypes.h"

// Sibling/Children includes
#include "Vector3.h"

struct Vector4
{
#if !APIABSTRACTION_PS3 && !PE_PLAT_IS_PS4
#pragma warning(disable : 4201)
#endif
	union{
		struct {
			PrimitiveTypes::Float32 m_x, m_y, m_z, m_w;
		};
		struct {
			PrimitiveTypes::Float32 m_r, m_g, m_b, m_a;
		};
		PrimitiveTypes::Float32 m_values[4];
	};
#if !APIABSTRACTION_PS3 && !PE_PLAT_IS_PS4
#pragma warning(default : 4201)
#endif
	Vector4() : m_x(0), m_y(0), m_z(0), m_w(0)
	{}

	Vector4(PrimitiveTypes::Float32 x, PrimitiveTypes::Float32 y, PrimitiveTypes::Float32 z) : m_x(x), m_y(y), m_z(z), m_w(0)
	{}

	Vector4(PrimitiveTypes::Float32 x, PrimitiveTypes::Float32 y, PrimitiveTypes::Float32 z, PrimitiveTypes::Float32 w) : m_x(x), m_y(y), m_z(z), m_w(w)
	{}

	Vector3 & asVector3Ref() { return *((Vector3 *)(this));}
};

inline Vector4 operator+(const Vector4 &a, const Vector4 &b)
{
	return Vector4(a.m_x + b.m_x, a.m_y + b.m_y, a.m_z + b.m_z, a.m_w + b.m_w);
}

inline Vector4 operator/(const Vector4 &a, const float &f)
{
	return Vector4(a.m_x / f, a.m_y / f, a.m_z / f, a.m_w / f);
}

inline Vector4 operator*(const Vector4 &a, const float &f)
{
	return Vector4(a.m_x * f, a.m_y * f, a.m_z * f, a.m_w * f);
}

#endif
