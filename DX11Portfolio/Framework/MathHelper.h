#pragma once

#include <directxtk/SimpleMath.h>

namespace MathHelper
{
    using namespace DirectX::SimpleMath;

    // πÊ«‚ ∫§≈Õ
    extern const Vector3 Forward;
    extern const Vector3 Up;
    extern const Vector3 Left;
    extern const Vector3 Right;
    
    static inline Quaternion SlerpSafe(Quaternion a, Quaternion b, float t)
    {
        if (a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w < 0.0f) {
            b.x = -b.x; b.y = -b.y; b.z = -b.z; b.w = -b.w;
        }
        auto q = Quaternion::Slerp(a, b, t);
        q.Normalize();
        return q;
    }

}