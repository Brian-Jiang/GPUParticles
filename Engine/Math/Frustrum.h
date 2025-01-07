#pragma once

#include "PrimeEngine/Math/Vector3.h"

struct Frustrum {
    Vector3 m_nearNormal;
    Vector3 m_nearPoint;
    Vector3 m_farNormal;
    Vector3 m_farPoint;
    Vector3 m_leftNormal;
    Vector3 m_leftPoint;
    Vector3 m_rightNormal;
    Vector3 m_rightPoint;
    Vector3 m_topNormal;
    Vector3 m_topPoint;
    Vector3 m_bottomNormal;
    Vector3 m_bottomPoint;
};