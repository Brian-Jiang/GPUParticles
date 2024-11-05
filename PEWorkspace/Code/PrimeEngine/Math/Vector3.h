#ifndef __PYENGINE_2_0_VECTOR3_H__
#define __PYENGINE_2_0_VECTOR3_H__

// APIAbstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

// Outer-Engine includes
// Windows
#if APIABSTRACTION_D3D9 | APIABSTRACTION_D3D11
#define _USE_MATH_DEFINES
#include <math.h>
#endif

// Inter-Engine includes
#include "PrimeEngine/MemoryManagement/Handle.h"
#include "PrimeEngine/PrimitiveTypes/PrimitiveTypes.h"

// Sibling/Children includes
#include "MathHelpers.h"


class Vector2 {
public:
#if !APIABSTRACTION_PS3 && !PE_PLAT_IS_PS4
#pragma warning(disable : 4201)
#endif
	union {
		PrimitiveTypes::Float32 m_values[2];
		struct{
			PrimitiveTypes::Float32 m_x, m_y;
		};
	};
#if !APIABSTRACTION_PS3 && !PE_PLAT_IS_PS4
#pragma warning(default : 4201)
#endif

	Vector2(PrimitiveTypes::Float32 x, PrimitiveTypes::Float32 y) : m_x(x), m_y(y)
	{}

	Vector2() : m_x(0), m_y(0)
	{}

};

class Vector3 {
public:
#if !APIABSTRACTION_PS3 && !PE_PLAT_IS_PS4
#pragma warning(disable : 4201)
#endif
	union{
		PrimitiveTypes::Float32 m_values[3];
		struct{
			PrimitiveTypes::Float32 m_x, m_y, m_z;
		};
	};
#if !APIABSTRACTION_PS3 && !PE_PLAT_IS_PS4
#pragma warning(default : 4201)
#endif

	Vector3() : m_x(0), m_y(0), m_z(0){}
	Vector3(PrimitiveTypes::Float32 x, PrimitiveTypes::Float32 y, PrimitiveTypes::Float32 z) : m_x(x), m_y(y), m_z(z){}
	Vector3(const Vector3 &copy){m_x = copy.m_x; m_y = copy.m_y; m_z = copy.m_z;}
	
	PrimitiveTypes::Float32 getX(void) const {return m_x;}
	PrimitiveTypes::Float32 getY(void) const {return m_y;}
	PrimitiveTypes::Float32 getZ(void) const {return m_z;}

	PrimitiveTypes::Float32 length(){return sqrtf(m_x * m_x + m_y * m_y + m_z * m_z);}

	PrimitiveTypes::Float32 lengthSqr(){return (m_x * m_x + m_y * m_y + m_z * m_z);}
	
	void normalize() {*this /= length();}

	void resize(PrimitiveTypes::Float32 newLength)
	{
		float k = newLength / length();
		*this *= k;
	}

	void operator /= (const PrimitiveTypes::Float32 &f) {m_x /= f; m_y /= f; m_z /= f;}

	void operator *= (const PrimitiveTypes::Float32 &f) {m_x *= f; m_y *= f; m_z *= f;}

	void operator += (const Vector3 &v) {m_x += v.m_x; m_y+= v.m_y; m_z += v.m_z;}

	void operator -= (const Vector3 &v) {m_x -= v.m_x; m_y-= v.m_y; m_z -= v.m_z;}

	Vector3 operator-() const {return Vector3(-m_x, -m_y, -m_z);}

	float dotProduct(const Vector3 &v) { return m_x * v.m_x + m_y * v.m_y + m_z * v.m_z;}

	Vector3 crossProduct(const Vector3 &v) const
	{
		return Vector3(
			m_y * v.m_z - m_z * v.m_y,
			m_z * v.m_x - m_x * v.m_z,
			m_x * v.m_y - m_y * v.m_x);
	}

	Vector3 operator * (PrimitiveTypes::Float32 f) {return Vector3(m_x * f, m_y * f, m_z * f);}

	PrimitiveTypes::Float32 operator *(const Vector3 &v) {return dotProduct(v);}

	Vector3 projectionOnVector(const Vector3 &v)
	{
		Vector3 temp(v);
		temp.normalize();
		return Vector3(temp * dotProduct(temp));
	}

	PrimitiveTypes::Float32 projectionTimeOnVector(const Vector3 &v)
	{
		Vector3 temp(v);
		temp.normalize();
		return dotProduct(temp);
	}
};

inline Vector3 operator * (PrimitiveTypes::Float32 f, const Vector3 &v) {return Vector3(v.m_x * f, v.m_y * f, v.m_z * f);}

inline Vector3 operator / (const Vector3 &v, PrimitiveTypes::Float32 f) {return Vector3(v.m_x / f, v.m_y / f, v.m_z / f);}
inline Vector3 operator / (const Vector3 &v, const Vector3& v2) {return Vector3(v.m_x / v2.m_x, v.m_y / v2.m_y, v.m_z / v2.m_z);}

inline Vector3 operator + (const Vector3 &v0, const Vector3 &v1) 
{
	return Vector3(v0.m_x + v1.m_x, v0.m_y + v1.m_y, v0.m_z + v1.m_z);
}

inline Vector3 operator - (const Vector3 &v0, const Vector3 &v1)
{
	return Vector3(v0.m_x - v1.m_x,	v0.m_y - v1.m_y, v0.m_z - v1.m_z);
}

inline bool operator == (const Vector3 &v0, const Vector3 &v1)
{
	return compareFloats(v0.m_x, v1.m_x) 
		&& compareFloats(v0.m_y, v1.m_y) 
		&& compareFloats(v0.m_z, v1.m_z);
}

inline bool operator > (const Vector3 &v0, const Vector3 &v1) 
{
	if(compareFloats(v0.m_x, v1.m_x))
		if(compareFloats(v0.m_y, v1.m_y))
			if (compareFloats(v0.m_z, v1.m_z))
				return false;
			else
				return (v0.m_z > v1.m_z);
		else
			return (v0.m_y > v1.m_y);
	else
		return(v0.m_x > v1.m_x);
}

inline bool operator < (const Vector3 &v0, const Vector3 &v1) 
{
	if(compareFloats(v0.m_x, v1.m_x))
		if(compareFloats(v0.m_y, v1.m_y))
			if (compareFloats(v0.m_z, v1.m_z))
				return false;
			else
				return v0.m_z < v1.m_z;
		else
			return v0.m_y < v1.m_y;
	else
		return v0.m_x < v1.m_x;
}

struct Vector2i
{
	PrimitiveTypes::Int32 m_xi, m_yi;

	Vector2i() : m_xi(0), m_yi(0) {}
	Vector2i(PrimitiveTypes::Int32 xi, PrimitiveTypes::Int32 yi) : m_xi(xi), m_yi(yi) {}
};

#endif
