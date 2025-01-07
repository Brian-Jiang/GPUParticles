#ifndef __PYENGINE_2_0_MATRIX_H__
#define __PYENGINE_2_0_MATRIX_H__


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
#include "Quaternion.h"

enum ERotateX
{
	RotateX
};

enum ERotateY
{
	RotateY
};

enum ERotateZ
{
	RotateZ
};

enum ERotate
{
	Rotate
};

enum ERotateOrder
{
	RotateOrder_XYZ,
	RotateOrder_ZYX

};

class Matrix3x3{
public:

	friend inline Matrix3x3 operator *(const Matrix3x3 &m0, const Matrix3x3 &m1);

	float m[3][3];
	Matrix3x3(const Matrix3x3 &mn)
	{
		memcpy(this, &mn, sizeof(Matrix3x3));
	}
	
	Matrix3x3(ERotateX rotAxis, float angle)
	{
		m[0][0] = 1.0f;         m[0][1] = 0;            m[0][2] = 0;
		m[1][0] = 0;            m[1][1] = cosf(angle);   m[1][2] = sinf(angle);
		m[2][0] = 0;            m[2][1] = -sinf(angle);  m[2][2] = cosf(angle);
	}

	Matrix3x3(ERotateY rotAxis, float angle)
	{
		m[0][0] = cosf(angle);   m[0][1] = 0;            m[0][2] = -sinf(angle);
		m[1][0] = 0;             m[1][1] = 1.0f;         m[1][2] = 0;
		m[2][0] = sinf(angle);   m[2][1] = 0;            m[2][2] = cosf(angle);
	}

	Matrix3x3(ERotateZ rotAxis, float angle)
	{
		m[0][0] = cosf(angle);   m[0][1] = sinf(angle);   m[0][2] = 0;
		m[1][0] = -sinf(angle);  m[1][1] = cosf(angle);   m[1][2] = 0;
		m[2][0] = 0;             m[2][1] = 0;             m[2][2] = 1.0f;
	}

	Matrix3x3(ERotate rot, Vector3 r, ERotateOrder rotOrder = RotateOrder_XYZ)
	{
		Matrix3x3 rx(RotateX, r.m_x);
		Matrix3x3 ry(RotateY, r.m_y);
		Matrix3x3 rz(RotateZ, r.m_z);
		if (rotOrder == RotateOrder_XYZ)
			*this = rx * ry * rz;
		else
			*this = rz * ry * rx;
	}

	Matrix3x3()
	{
		m[0][0] = 0; m[0][1] = 0; m[0][2] = 0;
		m[1][0] = 0; m[1][1] = 0; m[1][2] = 0;
		m[2][0] = 0; m[2][1] = 0; m[2][2] = 0;
	}
	
	void clear()
	{
		m[0][0] = 0; m[0][1] = 0; m[0][2] = 0;
		m[1][0] = 0; m[1][1] = 0; m[1][2] = 0;
		m[2][0] = 0; m[2][1] = 0; m[2][2] = 0;
	}

	/*
	Matrix3x3(float turnAngleX)
	{
		clear();
		m[0][0] = 1; m[0][1] = 0; m[0][2] = 0;
		m[1][0] = 0; m[1][1] = cos(turnAngleX); m[1][2] = sin(turnAngleX);
		m[2][0] = 0; m[2][1] = -sin(turnAngleX); m[2][2] = cos(turnAngleX);
	}
	*/

	void importScale(float x, float y, float z)
	{
		m[0][0] = (x);
		m[1][1] = (y);
		m[2][2] = (z);
	}

	Matrix3x3(const float x00, const float x01, const float x02,
		      const float x10, const float x11, const float x12,
			  const float x20, const float x21, const float x22)
	{
		m[0][0] = x00; m[0][1] = x01; m[0][2] = x02;
		m[1][0] = x10; m[1][1] = x11; m[1][2] = x12;
		m[2][0] = x20; m[2][1] = x21; m[2][2] = x22;
	}
	
	
	Matrix3x3(const Quaternion &q)
	{
		setFromQuat(q);
	}
	
	
	Matrix3x3(Vector3 u, Vector3 v, Vector3 n)
    {
        m[0][0] = u.m_x;
        m[0][1] = v.m_x;
        m[0][2] = n.m_x;
        
        m[1][0] = u.m_y;
        m[1][1] = v.m_y;
        m[1][2] = n.m_y;
        
        m[2][0] = u.m_z;
        m[2][1] = v.m_z;
        m[2][2] = n.m_z;
	
		/*
		m[0][0] = u._x;
        m[0][1] = u._y;
		m[0][2] = u._z;
        
        m[1][0] = v._x;
        m[1][1] = v._y;
        m[1][2] = v._z;
        
        m[2][0] = n._x;
        m[2][1] = n._y;
        m[2][2] = n._z;
		*/
    }
   
	void setValues(const float x00, const float x01, const float x02,
		           const float x10, const float x11, const float x12,
			       const float x20, const float x21, const float x22)
	{
		m[0][0] = x00; m[0][1] = x01; m[0][2] = x02;
		m[1][0] = x10; m[1][1] = x11; m[1][2] = x12;
		m[2][0] = x20; m[2][1] = x21; m[2][2] = x22;
	}

	void setFromQuat(const Quaternion &q)
	{
		m[0][0] = 1 - 2 * (q.m_y * q.m_y + q.m_z * q.m_z);
		m[0][1] = 2 * (q.getX() * q.getY() - q.getW() * q.getZ());
		m[0][2] = 2 * (q.getX() * q.getZ() + q.getW() * q.getY());
		
		m[1][0] = 2 * (q.getX() * q.getY() + q.getW() * q.getZ());
		m[1][1] = 1 - 2 * (q.getX() * q.getX() + q.getZ() * q.getZ());
		m[1][2] = 2 * (q.getY() * q.getZ() - q.getW() * q.getX());
		
		m[2][0] = 2 * (q.getX() * q.getZ() - q.getW() * q.getY());
		m[2][1] = 2 * (q.getY() * q.getZ() + q.getW() * q.getX());
		m[2][2] = 1 - 2 * (q.getX() * q.getX() + q.getY() * q.getY());
	}
	

	Vector3 multVector3(const Vector3 &v) const
	{
		Vector3 res(
		/*	v.getX() * m[0][0] + v.getY() * m[1][0] + v.getZ() * m[2][0],
			v.getX() * m[0][1] + v.getY() * m[1][1] + v.getZ() * m[2][1],
			v.getX() * m[0][2] + v.getY() * m[1][2] + v.getZ() * m[2][2]
		*/
			v.m_x * m[0][0] + v.m_y * m[0][1] + v.m_z * m[0][2],
			v.m_x * m[1][0] + v.m_y * m[1][1] + v.m_z * m[1][2],
			v.m_x * m[2][0] + v.m_y * m[2][1] + v.m_z * m[2][2]
		
        );
		return res;
	}
	
    /*    m_vals[0, 0] * v.getX() + m_vals[0, 1] * v.getY() + m_vals[0, 2] * v.getZ(),
        m_vals[1, 0] * v.getX() + m_vals[1, 1] * v.getY() + m_vals[1, 2] * v.getZ(),
        m_vals[2, 0] * v.getX() + m_vals[2, 1] * v.getY() + m_vals[2, 2] * v.getZ()
    */
    
	Vector3 multVector3( const float x, const float y, const float z) const
	{
		return multVector3(Vector3(x, y, z));
		
	}
	void addMatrix3x3(const Matrix3x3 &m)
	{
		this->m[0][0] += m.m[0][0]; this->m[0][1] += m.m[0][1]; this->m[0][2] += m.m[0][2];
		this->m[1][0] += m.m[1][0]; this->m[1][1] += m.m[1][1]; this->m[1][2] += m.m[0][2];
		this->m[2][0] += m.m[2][0]; this->m[2][1] += m.m[2][1]; this->m[2][2] += m.m[0][2];
	}

	Matrix3x3 plusMatrix3x3(const Matrix3x3 &m)
	{
		Matrix3x3 res(*this);
		res.addMatrix3x3(m);

		return res;
	}
	
	float det() const
	{
		return m[0][0] * m[1][1] * m[2][2] + m[0][1] * m[1][2] * m[2][0] + m[0][2] * m[1][0] * m[2][1]
			- m[0][2] * m[1][1] * m[2][0] - m[0][1] * m[1][0] * m[2][2] - m[0][0] * m[1][2] * m[2][1];
	}
	float matMinor(int y, int x) const
	{
		float min[2][2];
		int xMin, yMin = 0;

		for(int yM = 0; yM < 3; yM++){
			if(yM != y){
				xMin = 0;
				for(int xM = 0; xM < 3; xM++){
					if(xM != x){
						min[yMin][xMin] = m[yM][xM];
						xMin++;
					}
				}
				yMin++;
			}
		}

		return min[0][0] * min[1][1] - min[0][1] * min[1][0];
	}

	float furfiller(int y, int x) const
	{
		float min = matMinor(y, x);
		min *= powf(-1.0f, (y + x + 2.0f));

		return min;
	}
	
	Matrix3x3 transponentFurfiller() const
	{
		Matrix3x3 res;

		for(int y = 0; y < 3; y++)
			for(int x = 0; x < 3; x++)
				res.m[x][y] = furfiller(y, x);

		return res;
	}

	Matrix3x3 inverse() const
	{
		Matrix3x3 t = transponentFurfiller(),
			res;

		for(int y = 0; y < 3; y++)
			for(int x = 0; x < 3; x++)
				res.m[y][x] = t.m[y][x] / det();

		return res;
	}

};

inline Matrix3x3 operator *(const Matrix3x3 &m0, const Matrix3x3 &m1)
{
	Matrix3x3 res;
	res.clear();
	
	for(int row = 0; row < 3; row++){
		for(int column = 0; column < 3; column++){
			for(int k = 0; k < 3; k++){
				float s = m0.m[row][k] * m1.m[k][column];
				res.m[row][column] += s;
			}
		}
	}
	return res;
}

inline Vector3 operator *(const Matrix3x3 &m, const Vector3 &v)
{
	/*
	Vector3 res;
	for(int i = 0; i < 1; i++){
		for(int j = 0; j < 3; j++){
			for(int k = 0; k < 3; k++){

				float s = m0._values[k] * m1.m[k][j];
				res._values[j] += s;
			}
		}
	}
	return res;
	*/
	return m.multVector3(v);
}

#endif
