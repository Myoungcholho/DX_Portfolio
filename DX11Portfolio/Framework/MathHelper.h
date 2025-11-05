#pragma once
#define M_PI 

#include <directxtk/SimpleMath.h>

namespace MathHelper
{
    using namespace DirectX::SimpleMath;

    // 방향 벡터
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

    static float NormSq(const Quaternion& q) 
    {
        return q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w;
    }

    inline Quaternion IdentityQ() { return { 0,0,0,1 }; }

    inline Quaternion Multiply(const Quaternion& a, const Quaternion& b)
    {
        return Quaternion(
            a.w * b.x + a.x * b.w + a.y * b.z - a.z * b.y,  // x
            a.w * b.y - a.x * b.z + a.y * b.w + a.z * b.x,  // y
            a.w * b.z + a.x * b.y - a.y * b.x + a.z * b.w,  // z
            a.w * b.w - a.x * b.x - a.y * b.y - a.z * b.z   // w
        );
    }

    inline Quaternion Normalize(Quaternion& q)
    {
        float n = sqrtf(NormSq(q));
        if (n <= 0.0f)
            return IdentityQ();
        float inv = 1.0f / sqrt(n);

        return { q.x * inv, q.y * inv, q.z * inv, q.w * inv };
    }

    // Quaternion/Matrix 유틸 (엔진 규약에 맞게 교체)
    static inline Quaternion QNormalize(const Quaternion& q) {
        Quaternion r = q; r.Normalize(); return r;
    }
    static inline Quaternion QInverseUnit(const Quaternion& qUnit) {
        Quaternion c; qUnit.Conjugate(c); return c; // 단위 쿼터니언 가정
    }

    // yaw(Up/Y축)만 추출/적용 — 반드시 엔진 축/오더와 일치시킬 것!
    static inline double YawFromQ_Y(const Quaternion& q) 
    {
        // 쿼터니언 → 회전행렬 → 전방벡터(+Z)를 월드에 투영
        Matrix R = Matrix::CreateFromQuaternion(q);
        Vector3 f = Vector3::Transform(Vector3(0, 0, 1), R); // world forward

        // 수평면(Y-up)이므로 y 제거
        double x = f.x, z = f.z;
        double r2 = x * x + z * z;
        if (r2 < 1e-12) return 0.0; // 거의 수직일 때 안전처리

        // LH(+Z forward)에서는 atan2(x, z)가 요(yaw)
        return std::atan2(x, z);
    }
    static inline Quaternion YawQuat_Y(double yawRad) 
    {
        double h = 0.5 * yawRad;
        return Quaternion(0, std::sin(h), 0, std::cos(h));
    }
    static inline Quaternion RemoveYaw_Y(const Quaternion& q) 
    {
        double yaw = YawFromQ_Y(q);          // 위에서 뽑은 yaw와 동일 기준
        Quaternion qYaw = YawQuat_Y(yaw);
        // DirectX(SimpleMath)는 보통 "좌곱 규약"을 씀: q_total ≈ qYaw * qPitchRoll
        // pitch/roll만 남기려면 invYaw * q 가 맞다.
        Quaternion invYaw; qYaw.Conjugate(invYaw);
        Quaternion pr = invYaw * q;
        pr.Normalize();
        return pr;
    }

    // yaw 언랩(연속화) — 디버그 누적용
    static inline double UnwrapAdd(double prev, double d) {
        const double twoPi = 6.283185307179586;
        double raw = prev + d, diff = raw - prev;
        while (diff > 3.14159265f) diff -= twoPi;
        while (diff < -3.14159265f) diff += twoPi;
        return prev + diff;
    }
    constexpr double kPI = 3.1415926535897932384626433832795;
    constexpr double kTwoPI = 6.283185307179586476925286766559;

    // -π ~ +π 로 각도를 감싸기 (radian)
    inline double WrapPi(double a)
    {
        // a를 (-π, π]로 매핑
        a = std::fmod(a + kPI, kTwoPI);
        if (a < 0.0) a += kTwoPI;
        return a - kPI;
    }

    // 최단각으로 정리 (radian)
    inline double Unwrap(double deltaRad)
    {
        return WrapPi(deltaRad);
    }

    // float 버전이 필요하면 아래처럼 오버로드 추가
    inline float WrapPi(float a)
    {
        const float TwoPI = static_cast<float>(kTwoPI);
        const float PI = static_cast<float>(kPI);
        a = std::fmod(a + PI, TwoPI);
        if (a < 0.0f) a += TwoPI;
        return a - PI;
    }

    inline float Unwrap(float d) { return WrapPi(d); }
    inline float UnwrapAdd(float acc, float d) { return acc + WrapPi(d); }

    // -------------------------------------------------------------------------
    // Vector를 Quaternion으로 회전시키는 함수
    // v' = q * v * q^-1
    // -------------------------------------------------------------------------
    inline Vector3 RotateVectorByQuaternion(const Vector3& vector, const Quaternion& rotation)
    {
        XMVECTOR v = XMLoadFloat3(&vector);
        XMVECTOR q = XMLoadFloat4(&rotation);

        // XMVector3Rotate는 내부적으로 q * v * q^-1 수행
        XMVECTOR rotated = DirectX::XMVector3Rotate(v, q);

        Vector3 result;
        XMStoreFloat3(&result, rotated);
        return result;
    }
}