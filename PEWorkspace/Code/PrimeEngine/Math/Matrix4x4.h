#ifndef __PYENGINE_2_0_MATRIX4x4_H__
#define __PYENGINE_2_0_MATRIX4x4_H__


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
#include "PrimeEngine/../../GlobalConfig/GlobalConfig.h"

// Sibling/Children includes
#include "MathHelpers.h"
#include "Vector3.h"
#include "Vector4.h"

#include "Quaternion.h"
#include "Matrix3x3.h"
#if PE_PLAT_IS_WIN32
#include <xmmintrin.h>
#include <pmmintrin.h>
#endif
struct Row4{
public:
	float m0, m1, m2, m3;

	Row4(float x0, float x1, float x2, float x3)
	{
		m0 = x0; m1 = x1; m2 = x2; m3 = x3;
	}

};

struct Matrix4x4{
	union
	{
		float m[4][4];
		float m16[16];
#if PE_PLAT_IS_WIN32
		__m128 mSSERows[4];
#endif
	};

    void clear()
    {
        memset((void *)m, 0, sizeof(m));
    }
	void loadIdentity()
	{
		clear();
		m[0][0] = m[1][1] = m[2][2] = m[3][3] = 1.0f;
	}

	
	Matrix4x4(Quaternion q)
	{
		clear();
		Matrix3x3 m3x3(q);
		for(int y = 0; y < 3; y++)
			for(int x = 0; x < 3; x++)
				this->m[y][x] = m3x3.m[y][x];
		m[3][3] = 1.0;
	}

	Matrix4x4(const Matrix4x4 &copy)
    {
        for(int y = 0; y < 4; y++)
            for(int x = 0; x < 4; x++)
                m[y][x] = copy.m[y][x];
	}
/*	Matrix4x4(Matrix4x4 copy)
    {
        for(int y = 0; y < 4; y++)
            for(int x = 0; x < 4; x++)
                m[y][x] = copy.m[y][x];
	}

*/

	void setFromQuatAndPos(const Quaternion &q, const Vector3 &pos)
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

		m[0][3] = pos.m_x;
		m[1][3] = pos.m_y;
		m[2][3] = pos.m_z;

		m[3][0] = 0;
		m[3][1] = 0;
		m[3][2] = 0;
		m[3][3] = 1.0f;
	}

	Matrix4x4(const float x00, const float x01, const float x02, const float x03,
		      const float x10, const float x11, const float x12, const float x13,
			  const float x20, const float x21, const float x22, const float x23,
			  const float x30, const float x31, const float x32, const float x33)
	{
		m[0][0] = x00; m[0][1] = x01; m[0][2] = x02;m[0][3] = x03;
		m[1][0] = x10; m[1][1] = x11; m[1][2] = x12;m[1][3] = x13;
		m[2][0] = x20; m[2][1] = x21; m[2][2] = x22;m[2][3] = x23;
		m[3][0] = x30; m[3][1] = x31; m[3][2] = x32;m[3][3] = x33;
	}

	Matrix4x4(float vals[16])
	{
		m[0][0] = vals[0]; m[1][0] = vals[1]; m[2][0] = vals[2];m[3][0] = vals[3];
		m[0][1] = vals[4]; m[1][1] = vals[5]; m[2][1] = vals[6];m[3][1] = vals[7];
		m[0][2] = vals[8]; m[1][2] = vals[9]; m[2][2] = vals[10];m[3][2] = vals[11];
		m[0][3] = vals[12]; m[1][3] = vals[13]; m[2][3] = vals[14];m[3][3] = vals[15];
	}

	void toFloat16(float vals[16])
	{
		vals[0] = m[0][0]; vals[1]  = m[1][0]; vals[2]  = m[2][0]; vals[3]  = m[3][0];
		vals[4] = m[0][1]; vals[5]  = m[1][1]; vals[6]  = m[2][1]; vals[7]  = m[3][1];
		vals[8] = m[0][2]; vals[9]  = m[1][2]; vals[10] = m[2][2]; vals[11] = m[3][2];
		vals[12]= m[0][3]; vals[13] = m[1][3]; vals[14] = m[2][3]; vals[15] = m[3][3];
	}
	
	Matrix4x4(const Matrix3x3 &m3x3, const Vector3 &pos)
	{

		// x-axis                    y-axis                    z-axis                    translate
		m[0][0] = m3x3.m[0][0];      m[0][1] = m3x3.m[0][1];   m[0][2] = m3x3.m[0][2];   m[0][3] = pos.m_x;
		m[1][0] = m3x3.m[1][0];      m[1][1] = m3x3.m[1][1];   m[1][2] = m3x3.m[1][2];   m[1][3] = pos.m_y;
		m[2][0] = m3x3.m[2][0];      m[2][1] = m3x3.m[2][1];   m[2][2] = m3x3.m[2][2];   m[2][3] = pos.m_z;

		m[3][0] = 0;                 m[3][1] = 0;              m[3][2] = 0;              m[3][3] = 1.0f;
	}


	/*
	Matrix4x4(Vector3 u, Vector3 v, Vector3 n)
    {
        clear();
        Matrix3x3 m3x3(u, v, n);
        for(int y = 0; y < 3; y++)
            for(int x = 0; x < 3; x++)
                m[y][x] = m3x3.m[y][x];
        m[3][3] = 1.0;
    }
	*/

	explicit Matrix4x4(Vector3 pos)
    {
        loadIdentity();
		/*
        m[0][3] = pos._x;
		m[1][3] = pos._y;
		m[2][3] = pos._z;
		*/
		m[0][3] = pos.m_x;
		m[1][3] = pos.m_y;
		m[2][3] = pos.m_z;
		
	}

	Vector3 getU() const {return Vector3(m[0][0], m[1][0], m[2][0]);}
	Vector3 getV() const {return Vector3(m[0][1], m[1][1], m[2][1]);}
	Vector3 getN() const {return Vector3(m[0][2], m[1][2], m[2][2]);}

	Vector3 getPos() const {return Vector3(m[0][3], m[1][3], m[2][3]);}
	
	void setU(Vector3 u)
	{
		m[0][0] = u.m_x;
		m[1][0] = u.m_y;
		m[2][0] = u.m_z;
	}
	
	void setV(Vector3 v)
	{
		m[0][1] = v.m_x;
		m[1][1] = v.m_y;
		m[2][1] = v.m_z;
	}

	void setN(Vector3 n)
	{
		m[0][2] = n.m_x;
		m[1][2] = n.m_y;
		m[2][2] = n.m_z;
	}

	void scaleU(float len)
	{
		float &x = m[0][0]; float &y = m[1][0]; float &z = m[2][0];
		float mult = len / sqrtf(x * x + y * y + z * z);
		x *= mult; y *= mult; z *= mult;
	}

	void scaleV(float len)
	{
		float &x = m[0][1]; float &y = m[1][1]; float &z = m[2][1];
		float mult = len / sqrtf(x * x + y * y + z * z);
		x *= mult; y *= mult; z *= mult;
	}

	void scaleN(float len)
	{
		float &x = m[0][2]; float &y = m[1][2]; float &z = m[2][2];
		float mult = len / sqrtf(x * x + y * y + z * z);
		x *= mult; y *= mult; z *= mult;
	}

	void normalizeUVN()
	{
		scaleU(1.0f);
		scaleV(1.0f);
		scaleN(1.0f);
	}
	
	void setPos(Vector3 p)
	{
		m[0][3] = p.m_x;
		m[1][3] = p.m_y;
		m[2][3] = p.m_z;
	}

	void moveForward(float distance)
	{
//#		if APIABSTRACTION_OGL
			setPos(getPos() + getN() * distance);
//#		else
//			setPos(getPos() - getN() * distance);
//#		endif
	}

	void moveBack(float distance)
	{
//#		if APIABSTRACTION_OGL
			setPos(getPos() - getN() * distance);
//#		else
//			setPos(getPos() + getN() * distance);
//#		endif
	}

	void moveRight(float distance) {setPos(getPos() + getU() * distance);}
	void moveLeft(float distance) {setPos(getPos() - getU() * distance);}

	void moveUp(float distance) {setPos(getPos() + getV() * distance);}
	void moveDown(float distance) {setPos(getPos() - getV() * distance);}

	void turnRight(float angle)
	{
		Quaternion axis(getV());
#if APIABSTRACTION_OGL
		setU(axis.turnV3DHandDir(-angle, getU()));
		setN(axis.turnV3DHandDir(-angle, getN()));
#else
		setU(axis.turnV3DHandDir(-angle, getU()));
		setN(axis.turnV3DHandDir(-angle, getN()));
#endif

	}

	void turnLeft(float angle)	{
		Quaternion axis(getV());
#if APIABSTRACTION_OGL
		setU(axis.turnV3DHandDir(angle, getU()));
		setN(axis.turnV3DHandDir(angle, getN()));
#else
		setU(axis.turnV3DHandDir(angle, getU()));
		setN(axis.turnV3DHandDir(angle, getN()));
#endif
	}

	void turnUp(float angle)
	{
		Quaternion axis(getU());
		setV(axis.turnV3DHandDir(angle, getV()));
		setN(axis.turnV3DHandDir(angle, getN()));
	}

	void turnAboutAxis(float angle, Vector3 vectAxis)
	{
		Quaternion axis(vectAxis);
		setU(axis.turnV3DHandDir(angle, getU()));
		setV(axis.turnV3DHandDir(angle, getV()));
		setN(axis.turnV3DHandDir(angle, getN()));
	}

	void turnDown(float angle)
	{
		Quaternion axis(getU());
		setV(axis.turnV3DHandDir(-angle, getV()));
		setN(axis.turnV3DHandDir(-angle, getN()));
	}

	void rollRight(float angle)
	{
		Quaternion axis(getN());
		setU(axis.turnV3DHandDir(-angle, getU()));
		setV(axis.turnV3DHandDir(-angle, getV()));
	}

	void rollLeft(float angle)
	{
		Quaternion axis(getN());
		setU(axis.turnV3DHandDir(angle, getU()));
		setV(axis.turnV3DHandDir(angle, getV()));
	}

	PrimitiveTypes::Float32 angleToTurnTo(Vector3 targetPos, PrimitiveTypes::Bool &out_turnRight)
	{
		Vector3 forward = getN();
		forward.m_y = 0.0f;
		forward.normalize();
		Vector3 newForward = targetPos - getPos();
		newForward.m_y = 0.0f;
		newForward.normalize();

		PrimitiveTypes::Float32 cosAlpha = forward * newForward;
		if (cosAlpha > 1.0f)
			cosAlpha = 1.0f;
		
		if (cosAlpha < -1.0f)
			cosAlpha = -1.0f;

		PrimitiveTypes::Float32 sinAlpha = forward.crossProduct(newForward).m_y;

		PrimitiveTypes::Float32 alpha = acosf(cosAlpha);

		if (fabs(alpha) < 0.001)
		{
			// looking at target
			out_turnRight = false;
			return 0.0f;
		}
		
		if (sinAlpha < 0)
		{
			// need to turn left by alpha
			out_turnRight = false;
			return alpha;
		}
		else
		{
			// need to turn right by alpha
			out_turnRight = true;
			return alpha;
		}
	}
	
	void turnTo(Vector3 targetPos)
	{
		Vector3 forward = getN();
		forward.m_y = 0.0f;
		forward.normalize();
		Vector3 newForward = targetPos - getPos();
		if (newForward.length() < 0.0001)
			return;
		newForward.m_y = 0.0f;
		newForward.normalize();

		PrimitiveTypes::Float32 cosAlpha = forward * newForward;
		if (cosAlpha > 1.0f)
			cosAlpha = 1.0f;
		
		if (cosAlpha < -1.0f)
			cosAlpha = -1.0f;

		PrimitiveTypes::Float32 sinAlpha = forward.crossProduct(newForward).m_y;

		PrimitiveTypes::Float32 alpha = acosf(cosAlpha);

		if (fabs(alpha) < 0.001)
		{
			// looking at target
			return;
		}
		
		if (sinAlpha < 0)
		{
			// need to turn left by alpha
			turnLeft(alpha);
		}
		else
		{
			// need to turn right by alpha
			turnRight(alpha);
		}
	}
	void turnInDirection(Vector3 targetDirection)
	{
		Vector3 forward = getN();
		forward.m_y = 0.0f;
		forward.normalize();
		Vector3 newForward = targetDirection;
		newForward.m_y = 0.0f;
		newForward.normalize();

		PrimitiveTypes::Float32 cosAlpha = forward * newForward;
		if (cosAlpha > 1.0f)
			cosAlpha = 1.0f;
		
		if (cosAlpha < -1.0f)
			cosAlpha = -1.0f;

		PrimitiveTypes::Float32 sinAlpha = forward.crossProduct(newForward).m_y;

		PrimitiveTypes::Float32 alpha = acosf(cosAlpha);

		if (fabs(alpha) < 0.001)
		{
			// looking at target
			return;
		}
		
		if (sinAlpha < 0)
		{
			// need to turn left by alpha
			turnLeft(alpha);
		}
		else
		{
			// need to turn right by alpha
			turnRight(alpha);
		}
	}

	// Will return true if reached desired direction
	bool turnInDirection(Vector3 targetDirection, PrimitiveTypes::Float32 maxAngle, PrimitiveTypes::Float32 &turnedAngle)
	{
		Vector3 forward = getN();
		forward.m_y = 0.0f;
		forward.normalize();
		Vector3 newForward = targetDirection;
		newForward.m_y = 0.0f;
		newForward.normalize();

		PrimitiveTypes::Float32 cosAlpha = forward * newForward;
		if (cosAlpha > 1.0f)
			cosAlpha = 1.0f;
		
		if (cosAlpha < -1.0f)
			cosAlpha = -1.0f;

		PrimitiveTypes::Float32 sinAlpha = forward.crossProduct(newForward).m_y;

		PrimitiveTypes::Float32 alpha = acosf(cosAlpha);

		if (fabs(alpha) < 0.001)
		{
			// looking at target
			return true; // reached it
		}

		bool result = false;

		if (alpha < 0)
		{
			if (alpha < -maxAngle)
			{
				alpha = -maxAngle;
				result = false; // can't reach desired angle orientation
			}
			else
			{
				result = true;
			}
		}
		else
		{
			if (alpha > maxAngle)
			{
				alpha = maxAngle;
				result = false; // can't reach desired angle orientation
			}
			else
			{
				result = true;
			}
		}
		
		if (sinAlpha < 0)
		{
			// need to turn left by alpha
			turnLeft(alpha);
			turnedAngle = alpha;
		}
		else
		{
			// need to turn right by alpha
			turnRight(alpha);
			turnedAngle = -alpha;
		}

		return result;
	}

	// Will return true if reached desired direction
	bool turnInDirection(Vector3 targetDirection, PrimitiveTypes::Float32 maxAngle)
	{
		PrimitiveTypes::Float32 turnedAngle;
		return turnInDirection(targetDirection, maxAngle, turnedAngle);
	}
	

	PrimitiveTypes::Float32 angleToTurnToWithBack(Vector3 targetPos, PrimitiveTypes::Bool &out_turnRight)
	{
		Vector3 targetForN = getPos() + -(targetPos - getPos());
		return angleToTurnTo(targetForN, out_turnRight);
	}
	

	void turnToWithBack(Vector3 targetPos)
	{
		Vector3 targetForN = getPos() + -(targetPos - getPos());
		turnTo(targetForN);
	}
	
	Quaternion createQuat() const
	{
		Quaternion res;
		float trace = m[0][0] + m[1][1] + m[2][2]; // I removed + 1.0f; see discussion with Ethan
		if( trace > 0 ) {// I changed M_EPSILON to 0
			float s = 0.5f / sqrtf(trace + 1.0f);
			res.m_w = 0.25f / s;
			res.m_x = ( m[2][1] - m[1][2] ) * s;
			res.m_y = ( m[0][2] - m[2][0] ) * s;
			res.m_z = ( m[1][0] - m[0][1] ) * s;
		} else {
			if ( m[0][0] > m[1][1] && m[0][0] > m[2][2] ) {
				float s = 2.0f * sqrtf( 1.0f + m[0][0] - m[1][1] - m[2][2]);
				res.m_w = (m[2][1] - m[1][2] ) / s;
				res.m_x = 0.25f * s;
				res.m_y = (m[0][1] + m[1][0] ) / s;
				res.m_z = (m[0][2] + m[2][0] ) / s;
			} else if (m[1][1] > m[2][2]) {
				float s = 2.0f * sqrtf( 1.0f + m[1][1] - m[0][0] - m[2][2]);
				res.m_w = (m[0][2] - m[2][0] ) / s;
				res.m_x = (m[0][1] + m[1][0] ) / s;
				res.m_y = 0.25f * s;
				res.m_z = (m[1][2] + m[2][1] ) / s;
			} else {
				float s = 2.0f * sqrtf( 1.0f + m[2][2] - m[0][0] - m[1][1] );
				res.m_w = (m[1][0] - m[0][1] ) / s;
				res.m_x = (m[0][2] + m[2][0] ) / s;
				res.m_y = (m[1][2] + m[2][1] ) / s;
				res.m_z = 0.25f * s;
			}
		}
		return res;
	}

#if 0
	Matrix4x4(Vector3 u, Vector3 v, Vector3 n, Vector3 pos)
    {
        clear();
		Matrix3x3 m3x3(u, v, n);
        for(int y = 0; y < 3; y++)
            for(int x = 0; x < 3; x++)
                m[y][x] = m3x3.m[y][x];
		m[3][3] = 1.0;

		m[0][3] = pos._x;
		m[1][3] = pos._y;
		m[2][3] = pos._z;

		/*
		m[3][0] = pos._x;
		m[3][1] = pos._y;
		m[3][2] = pos._z;
		*/

    }
#endif

	Matrix4x4() {loadIdentity();}
	
	void setRow(Row4 r, int row)
	{
		m[row][0] = r.m0;
		m[row][1] = r.m1;
		m[row][2] = r.m2;
		m[row][3] = r.m3;
	}

	void divRow(float d, int row)
	{
		m[row][0] /= d;
		m[row][1] /= d;
		m[row][2] /= d;
		m[row][3] /= d;
	}
	
	void importScale(float x, float y, float z)
	{
		m[0][0] = (x);
		m[1][1] = (y);
		m[2][2] = (z);
	}
	void importTranslate(float x, float y, float z)
	{
		/*
		m[3][0] = (x);
		m[3][1] = (y);
		m[3][2] = (z);
		*/

		m[0][3] = (x);
		m[1][3] = (y);
		m[2][3] = (z);
	}
	/*
    void convertToGl()
    {
        float t[4][4];
        for(int y = 0; y < 4; y++)
            for(int x = 0; x < 4; x++)
                t[x][y] = m[y][x];
        
        for(int y = 0; y < 4; y++)
            for(int x = 0; x < 4; x++)
               m[y][x] = t[y][x];
	}*/

	float matMinor(int y, int x) const
	{
		Matrix3x3 min;
		int xMin, yMin = 0;

		for(int yM = 0; yM < 4; yM++){
			if(yM != y){
				xMin = 0;
				for(int xM = 0; xM < 4; xM++){
					if(xM != x){
						min.m[yMin][xMin] = m[yM][xM];
						xMin++;
					}
				}
				yMin++;
			}
		}

		return min.det();
	}

	float furfiller(int y, int x) const
	{
		float min = matMinor(y, x);
		min *= powf(-1.0f, (y + x + 2.0f));

		return min;
	}
	
	Matrix4x4 transponentFurfiller() const
	{
		Matrix4x4 res;

		for(int y = 0; y < 4; y++)
			for(int x = 0; x < 4; x++)
				res.m[x][y] = furfiller(y, x);

		return res;
	}

	float det() const
	{
		float r0 = m[2][3] * m[3][2] * m[0][1] * m[1][0] - m[2][2] * m[3][3] * m[0][1] * m[1][0] - m[2][3] * m[3][1] * m[0][2] * m[1][0] + m[2][1] * m[3][3] * m[0][2] * m[1][0],
			r1 =m[2][2] * m[3][1] * m[0][3] * m[1][0] -  m[2][1] * m[3][2] * m[0][3] * m[1][0] - m[0][0] * m[2][3] * m[3][2] * m[1][1] + m[0][0] * m[2][2] * m[3][3] * m[1][1],
			r2 = m[2][3] * m[3][0] * m[0][2] * m[1][1] -  m[2][2] * m[3][0] * m[0][3] * m[1][1] + m[0][0] * m[2][3] * m[3][1] * m[1][2] - m[0][0] * m[2][1] * m[3][3] * m[1][2],
			r3 = m[2][3] * m[3][0] * m[0][1] * m[1][2] +  m[2][1] * m[3][0] * m[0][3] * m[1][2] - m[0][0] * m[2][2] * m[3][1] * m[1][3] + m[0][0] * m[2][1] * m[3][2] * m[1][3],
			r4 = m[2][2] * m[3][0] * m[0][1] * m[1][3] -  m[2][1] * m[3][0] * m[0][2] * m[1][3] - m[3][3] * m[0][2] * m[1][1] * m[2][0] + m[3][2] * m[0][3] * m[1][1] * m[2][0],
			r5 = m[3][3] * m[0][1] * m[1][2] * m[2][0] -  m[3][1] * m[0][3] * m[1][2] * m[2][0] - m[3][2] * m[0][1] * m[1][3] * m[2][0] + m[3][1] * m[0][2] * m[1][3] * m[2][0];
		return r0 + r1 + r2 - r3 + r4 + r5;

//		return m[0][0] * m[1][1] * m[2][2] * m[3][3] + m[0][1] * m[1][2] * m[2][3] * m[3][0] 
//			+ m[0][2] * m[1][3] * m[2][0] * m[3][1] + m[0][3] * m[1][0] * m[2][1] * m[3][2]
//			- m[0][2] * m[1][1] * m[2][0] * m[3][3] - m[0][1] * m[1][0] * m[2][3] * m[3][2] 
//			- m[0][0] * m[1][3] * m[2][2] * m[3][1] - m[0][3] * m[1][2] * m[2][1] * m[3][0];
	}
	
	Matrix4x4 transpose() const
	{
		Matrix4x4 res;
		for(int y = 0; y < 4; y++)
			for(int x = 0; x < 4; x++)
				res.m[y][x] = m[x][y];

		return res;
	}

	Matrix4x4 inverse() const
	{
		Matrix4x4 t(transponentFurfiller()),
			res;

		for(int y = 0; y < 4; y++)
			for(int x = 0; x < 4; x++)
				res.m[y][x] = t.m[y][x] / det();

		return res;
	}

	Matrix4x4 sqr()
	{
	Matrix4x4 res;
	res.clear();
	for(int i = 0; i < 4; i++){
		for(int j = 0; j < 4; j++){
			res.m[i][j] = m[i][j] * m[i][j];
		}
	}
	return res;
}
#if PE_PLAT_IS_WIN32
	void getTransformedByOther(const Matrix4x4 &m0)
	{
		// this is m1 in m0 * m1

		__m128 m1col[4];

		m1col[0] = mSSERows[0];
		m1col[1] = mSSERows[1];
		m1col[2] = mSSERows[2];
		m1col[3] = mSSERows[3];

		_MM_TRANSPOSE4_PS(m1col[0], m1col[1], m1col[2], m1col[3]);

		for(int row = 0; row < 4; row++){
			for(int column = 0; column < 4; column++){
				
				__m128 ress = _mm_mul_ps(m0.mSSERows[row], m1col[column]);

				ress = _mm_hadd_ps(ress, ress);
				ress = _mm_hadd_ps(ress, ress);

				float sum = ress.m128_f32[0];
				/*
				for(int k = 0; k < 4; k++){

					float s = m[row][k] * m1.m[k][column];
					res.m[row][column] += s;
				}*/
				m[row][column] = sum;
			}
		}
	}

void transformOtherAndStore(const Matrix4x4 &m1)
	{
		// this is m0 in m0 * m1

		__m128 m1col[4];

		m1col[0] = m1.mSSERows[0];
		m1col[1] = m1.mSSERows[1];
		m1col[2] = m1.mSSERows[2];
		m1col[3] = m1.mSSERows[3];

		__m128 mRowCpy[4];
		mRowCpy[0] = mSSERows[0];
		mRowCpy[1] = mSSERows[1];
		mRowCpy[2] = mSSERows[2];
		mRowCpy[3] = mSSERows[3];
		_MM_TRANSPOSE4_PS(m1col[0], m1col[1], m1col[2], m1col[3]);

		for(int row = 0; row < 4; row++){
			for(int column = 0; column < 4; column++){
				
				__m128 ress = _mm_mul_ps(mRowCpy[row], m1col[column]);

				ress = _mm_hadd_ps(ress, ress);
				ress = _mm_hadd_ps(ress, ress);

				float sum = ress.m128_f32[0];
				/*
				for(int k = 0; k < 4; k++){

					float s = m[row][k] * m1.m[k][column];
					res.m[row][column] += s;
				}*/
				m[row][column] = sum;
			}
		}
	}
#else
    void getTransformedByOther(const Matrix4x4 &m0)
	{
		*this = m0 * *this;
	}
    
    void transformOtherAndStore(const Matrix4x4 &m1)
	{
		// this is m0 in m0 * m1
        *this = *this * m1;
	}

#endif
	Matrix4x4 operator *(const Matrix4x4 &m1) const
	{
		#if	PE_PERFORM_REDUNDANCY_MEMORY_CHECKS
			PEASSERT((int)(this) % 16 == 0 && (int)(&m1) % 16 == 0, "Matrix memory not alligned!");
		#endif
		Matrix4x4 res;
        
#if PE_PLAT_IS_WIN32
	
		__m128 m1col[4];

		m1col[0] = m1.mSSERows[0];
		m1col[1] = m1.mSSERows[1];
		m1col[2] = m1.mSSERows[2];
		m1col[3] = m1.mSSERows[3];

		_MM_TRANSPOSE4_PS(m1col[0], m1col[1], m1col[2], m1col[3]);
#else
        res.clear();
        #endif
		for(int row = 0; row < 4; row++){
			for(int column = 0; column < 4; column++){
				
#if PE_PLAT_IS_WIN32
				__m128 ress = _mm_mul_ps(mSSERows[row], m1col[column]);

				ress = _mm_hadd_ps(ress, ress);
				ress = _mm_hadd_ps(ress, ress);

				float sum = ress.m128_f32[0];
                res.m[row][column] = sum;
#else
                
				for(int k = 0; k < 4; k++){

					float s = m[row][k] * m1.m[k][column];
					res.m[row][column] += s;
				}
#endif
				
			}
		}
		return res;
	}
	Vector3 operator *(const Vector3 &v) const
	{
		Vector3 res;
		float res3 = 0, src3 = 1.0;
		for(int row = 0; row < 4; row++){
			for(int j = 0; j < 4; j++){
				
					float s;
					if(j < 3)
						s = v.m_values[j] * m[row][j];
					else
						s = src3 * m[row][j];

					if(row < 3)
						res.m_values[row] += s;
					else
						res3 += s;
				}
		}
		
		for(int i = 0; i < 3; i++)
			res.m_values[i] /= res3;

		return res;
	}
};


inline Matrix4x4 operator +(const Matrix4x4 &m0, const Matrix4x4 &m1)
{
	Matrix4x4 res;
	res.clear();
	for(int row = 0; row < 4; row++){
		for(int column = 0; column < 4; column++){
			res.m[row][column] = m0.m[row][column] + m1.m[row][column];
			
		}
	}
	return res;
}

inline Matrix4x4 operator *(const Matrix4x4 &m0, const float &f)
{
	Matrix4x4 res;
	res.clear();
	for(int i = 0; i < 4; i++){
		for(int j = 0; j < 4; j++){
			res.m[i][j] = m0.m[i][j] * f;
			
		}
	}
	return res;
}

#endif
