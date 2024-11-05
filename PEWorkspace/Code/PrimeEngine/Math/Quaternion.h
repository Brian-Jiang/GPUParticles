#ifndef __PYENGINE_2_0_QUATERNION_H__
#define __PYENGINE_2_0_QUATERNION_H__

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
#include "Vector3.h"

struct Quaternion {

	PrimitiveTypes::Float32 m_w, m_x, m_y, m_z;

	PrimitiveTypes::Float32 getW() const {return m_w;}
	PrimitiveTypes::Float32 getX() const {return m_x;}
	PrimitiveTypes::Float32 getY() const {return m_y;}
	PrimitiveTypes::Float32 getZ() const {return m_z;}

	Quaternion() { m_w = 0; m_x = 0; m_y = 0; m_z = 0;}
	
	Quaternion(const Vector3 &v) {m_w = 0; m_x = v.m_x; m_y = v.m_y; m_z = v.m_z;}

	Quaternion(const PrimitiveTypes::Float32 w , const PrimitiveTypes::Float32 x, const PrimitiveTypes::Float32 y, 
		const PrimitiveTypes::Float32 z)
	{
		m_w = w; m_x = x; m_y = y; m_z = z;
	}

	Quaternion(const PrimitiveTypes::Float32 x, const PrimitiveTypes::Float32 y, const PrimitiveTypes::Float32 z)
	{
		m_w = 0; m_x = x; m_y = y; m_z = z;
	}

	Quaternion(const Quaternion &q)
	{
		m_w = q.m_w; m_x = q.m_x; m_y = q.m_y; m_z = q.m_z;
	}

	Quaternion(const Vector3 &axis, float angle)
	{
		m_w = 0; m_x = axis.m_x; m_y = axis.m_y; m_z = axis.m_z;
		importTurnAngle(angle);
	}

	PrimitiveTypes::Float32 norm() const {return sqrtf(m_w*m_w + m_x*m_x + m_y*m_y + m_z*m_z);}

	PrimitiveTypes::Float32 normSqr() const {return (m_w*m_w + m_x*m_x + m_y*m_y + m_z*m_z);}
	
	Quaternion sopr() const {return Quaternion(m_w, -m_x, -m_y, -m_z);}

	Quaternion operator / (const PrimitiveTypes::Float32 &f)
	{
		m_w /= f;
		m_x /= f;
		m_y /= f;
		m_z /= f;
		return *this;
	}

	Vector3 asVector3() const {return Vector3(m_x, m_y, m_z);}
	
	void normalize() {*this = *this / norm();}

	Quaternion operator *= (const Quaternion &q) {return mult(q);}

	Quaternion operator - () const {return Quaternion(-m_w, -m_x, -m_y, -m_z);}

	Quaternion operator + () const {return *this;}

	Quaternion operator ^ (const int i) const
	{
		if(i == -1)
			return multReverseElement();
		else
			return Quaternion();
	}

	Quaternion operator * () const {return Quaternion(m_w, -m_x, -m_y, -m_z);}


	Quaternion mult(const Quaternion &q)
	{
		Quaternion res(m_w * q.m_w - m_x * q.m_x - m_y * q.m_y - m_z * q.m_z,
		               m_y * q.m_z - m_z * q.m_y + m_w * q.m_x + m_x * q.m_w,
		               m_z * q.m_x - m_x * q.m_z + m_w * q.m_y + m_y * q.m_w,
		               m_x * q.m_y - m_y * q.m_x + m_w * q.m_z + m_z * q.m_w);

		return res;
	}

	Quaternion mult(const PrimitiveTypes::Float32 &f) const {return Quaternion(m_w * f, m_x * f, m_y * f, m_z * f);}

	Quaternion mult(const PrimitiveTypes::Float32 &f1, const PrimitiveTypes::Float32 &f2)
	{
		return Quaternion(m_w * f1, m_x * f2, m_y * f2, m_z * f2);
	}

	Quaternion mult(const PrimitiveTypes::Float32 &f1, const PrimitiveTypes::Float32 &f2, const PrimitiveTypes::Float32 &f3)
	{
		return Quaternion(m_w, m_x * f1, m_y * f2, m_z * f3);
	}

	Quaternion mult(const PrimitiveTypes::Float32 &f1, const PrimitiveTypes::Float32 &f2,
		const PrimitiveTypes::Float32 &f3, const PrimitiveTypes::Float32 &f4)
	{
		return Quaternion(m_w * f1, m_x * f2, m_y * f3, m_z * f4);
	}

	void setValues(const PrimitiveTypes::Float32 w, const PrimitiveTypes::Float32 x, const PrimitiveTypes::Float32 y,
		const PrimitiveTypes::Float32 z)
	{
		m_w = w; m_x = x; m_y = y; m_z = z;
	}

	Quaternion multReverseElement() const
	{
		Quaternion res;
		sopr() / normSqr();
		return res;
	}

	void importTurnAngle(const PrimitiveTypes::Float32 angle)
	{
		PrimitiveTypes::Float32 angleDiv2 = angle * 0.5f,
		      sinAngleDiv2 = sinf(angleDiv2);
		m_w = cosf(angleDiv2);
		m_x *= sinAngleDiv2;
		m_y *= sinAngleDiv2;
		m_z *= sinAngleDiv2;
	}

	Vector3 turnV3DHandDir(const PrimitiveTypes::Float32 angle, const Vector3 &v) const;
};

inline Quaternion operator * (const Quaternion &q1, const Quaternion &q2)
{
	Quaternion res(q1);
	return res.mult(q2);
}

inline Quaternion operator * (const Quaternion &q, const float &f)
{
	return q.mult(f);
}

inline Quaternion operator + (const Quaternion &q1, const Quaternion &q2)
{
	return Quaternion(q1.m_w + q2.m_w, q1.m_x + q2.m_x, q1.m_y + q2.m_y, q1.m_z + q2.m_z);
}

inline Vector3 Quaternion::turnV3DHandDir(const PrimitiveTypes::Float32 angle, const Vector3 &v) const
{
	Quaternion
		q(v),
		axis(*this);
	axis.normalize();
	axis.importTurnAngle(angle);
#ifdef LEFT_COORDINATE_SYSTEM
		q =  ((*axis) * q) * axis;
#else
		q =  (axis * q) * (*axis); 
#endif
	return q.asVector3();
}

inline Vector3 rotateCCW(const Quaternion &a, const Vector3 &v)
{
	Quaternion
		q(v),
		axis(a);
	axis.normalize();
	//axis.importTurnAngle(angle);
	#ifdef LEFT_COORDINATE_SYSTEM
		q = ((*axis) * q) * axis;
	#else
		q = (axis * q) * (*axis);
	#endif
	return q.asVector3();
}

inline Vector3 rotateCW(const Quaternion &a, const Vector3 &v)
{
	Quaternion
		q(v),
		axis(a);
	axis.normalize();
	//axis.importTurnAngle(angle);
	#ifdef LEFT_COORDINATE_SYSTEM
		q =  (axis * q) * (*axis);
	#else
		q = ((*axis) * q) * axis;
	#endif
	return q.asVector3();
}

#endif
