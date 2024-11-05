#ifndef __PYENGINE_2_0_CAMERA_OPS_H__
#define __PYENGINE_2_0_CAMERA_OPS_H__
// These functions abstract low level camera matrix creation

// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

#include "PrimeEngine/PrimitiveTypes/PrimitiveTypes.h"
#include "PrimeEngine/Math/Matrix4x4.h"
#include "PrimeEngine/Math/Vector3.h"
#include "PrimeEngine/Math/Frustrum.h"

namespace CameraOps
{
	Matrix4x4 CreateViewMatrix(Vector3 &pos, Vector3 &target, Vector3 &up);

	Matrix4x4 CreateProjectionMatrix(PrimitiveTypes::Float32 verticalFov, PrimitiveTypes::Float32 aspectRatio,
		PrimitiveTypes::Float32 nearClip, PrimitiveTypes::Float32 farClip);

    Frustrum CreateFrustrum(const Vector3& position, const Vector3& forward, const Vector3& up, PrimitiveTypes::Float32 verticalFov, PrimitiveTypes::Float32 aspectRatio,
		PrimitiveTypes::Float32 nearClip, PrimitiveTypes::Float32 farClip);
}; // namespace CameraOps

#endif
