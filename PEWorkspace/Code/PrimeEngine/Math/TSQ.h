#ifndef __PYENGINE_2_0_TSQ_H__
#define __PYENGINE_2_0_TSQ_H__

// APIAbstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

// Outer-Engine includes
// Windows
#if APIABSTRACTION_D3D9 | APIABSTRACTION_D3D11
#define _USE_MATH_DEFINES
#include <math.h>
#endif

// Inter-Engine includes

// Sibling/Children includes
#include "Vector4.h"
#include "Quaternion.h"
#include "Matrix4x4.h"

// This struct encodes Translation, Scale and Quaternion Transformation

struct TSQ 
{
	TSQ()
	{
	}

	TSQ(const Matrix4x4 &transf, const Vector3 &scale)
	{
		m_quat = transf.createQuat();
		m_translation = Vector4(transf.m[0][3], transf.m[1][3], transf.m[2][3], 0);
		m_scale = Vector4(scale.m_x, scale.m_y, scale.m_z, 0);
	}

	TSQ(const Matrix4x4 &_transf)
	{
		Matrix4x4 transf = _transf;
		// find scale
		Vector3 xAxis(transf.m[0][0], transf.m[1][0], transf.m[2][0]);
		Vector3 yAxis(transf.m[0][1], transf.m[1][1], transf.m[2][1]);
		Vector3 zAxis(transf.m[0][2], transf.m[1][2], transf.m[2][2]);

		float xScale = xAxis.length();
		float yScale = yAxis.length();
		float zScale = zAxis.length();
		xAxis.normalize();
		yAxis.normalize();
		zAxis.normalize();

		Vector3 scale(xScale, yScale, zScale);

		transf.m[0][0] = xAxis.m_x; transf.m[1][0] = xAxis.m_y; transf.m[2][0] = xAxis.m_z;
		transf.m[0][1] = yAxis.m_x; transf.m[1][1] = yAxis.m_y; transf.m[2][1] = yAxis.m_z;
		transf.m[0][2] = zAxis.m_x; transf.m[1][2] = zAxis.m_y; transf.m[2][2] = zAxis.m_z;

		m_quat = transf.createQuat();
		m_translation = Vector4(transf.m[0][3], transf.m[1][3], transf.m[2][3], 0);
		m_scale = Vector4(scale.m_x, scale.m_y, scale.m_z, 0);
	}

	Matrix4x4 createMatrix()
	{
		Matrix4x4 res(m_quat);
		Matrix4x4 scale;
		scale.importScale(m_scale.m_x, m_scale.m_y, m_scale.m_z);
		res = res * scale;
		res.setPos(Vector3(m_translation.m_x, m_translation.m_y, m_translation.m_z));
		return res;
	}

	static PrimitiveTypes::Float32 RotDiff(TSQ &a, TSQ &b)
	{
		PrimitiveTypes::Float32 res;
		res = sqrtf(
			(a.m_quat.m_x -  b.m_quat.m_x) * (a.m_quat.m_x -  b.m_quat.m_x)
			+ (a.m_quat.m_y -  b.m_quat.m_y) * (a.m_quat.m_y -  b.m_quat.m_y)
			+ (a.m_quat.m_z -  b.m_quat.m_z) * (a.m_quat.m_z -  b.m_quat.m_z)
			+ (a.m_quat.m_w -  b.m_quat.m_w) * (a.m_quat.m_w -  b.m_quat.m_w)
		);
		
		return res;
	}

	static TSQ LERP(TSQ &a, TSQ &b, float alpha)
	{
		float dot = a.m_quat.asVector3() * b.m_quat.asVector3() + a.m_quat.m_w * b.m_quat.m_w;
		Quaternion realB = b.m_quat;
		if (dot < 0)
		{
			realB.m_x = -realB.m_x;
			realB.m_y = -realB.m_y;
			realB.m_z = -realB.m_z;
			realB.m_w = -realB.m_w;
		}
		
		TSQ res;
		res.m_quat = a.m_quat * (1.0f - alpha) + realB * alpha;
		//res.m_quat = a.m_quat;
		// TODO: do we ned this?
		res.m_quat.normalize();

		res.m_translation = a.m_translation * (1.0f - alpha) + b.m_translation * alpha;
		res.m_scale = a.m_scale * (1.0f - alpha) + b.m_scale * alpha;

		return res;
	}

	static TSQ SLERP(TSQ &a, TSQ &b, float alpha)
	{
		float dot = a.m_quat.asVector3() * b.m_quat.asVector3() + a.m_quat.m_w * b.m_quat.m_w;
		Quaternion realB = b.m_quat;
		if (dot < 0)
		{
			dot = -dot;
			realB.m_x = -realB.m_x;
			realB.m_y = -realB.m_y;
			realB.m_z = -realB.m_z;
			realB.m_w = -realB.m_w;
		}
		
		if (dot > 0.9999f)
		{
			return LERP(a, b, alpha);
		}

		//if ((1.0 - ABS(cos_angle)) > EPSILON) {
	  
		float sinOmega = sqrtf(1.0f - dot * dot);

		float omega = atan2f(sinOmega, dot);
		float oneOverSinOmega = 1.0f / sinOmega;

		float k0 = sinf((1.0f - alpha) * omega) * oneOverSinOmega;
		float k1 = sinf(alpha * omega) * oneOverSinOmega;
		
		TSQ res;
		res.m_quat = a.m_quat * k0 + realB * k1;
		
		// this is needed if bad input data or accumulated floating point error
		// TODO: maybe do this only when needed
		res.m_quat.normalize();

		res.m_translation = a.m_translation * (1.0f - alpha) + b.m_translation * alpha;
		res.m_scale = a.m_scale * (1.0f - alpha) + b.m_scale * alpha;
		
		return res;
	}

	Quaternion m_quat;
	Vector4 m_scale;
	Vector4 m_translation;
};

#endif
