#include "Framework.h"
#include "Transform.h"

/// <summary>
/// 쿼터 -> 오일러 변환 시 튐 현상 방지를 위해 이전 값과 가까운 값으로 결정
/// </summary>
Vector3 ToEulerClosestTo(const Quaternion& q, const Vector3& previousEulerDeg)
{
	Vector3 eulerRad = q.ToEuler(); // 기존 변환 결과, 튀는 결과일 수 있음
	
	// 2) prev(deg) → rad
	Vector3 prevRad(
		XMConvertToRadians(previousEulerDeg.x),
		XMConvertToRadians(previousEulerDeg.y),
		XMConvertToRadians(previousEulerDeg.z)
	);

	// 3) 2π 래핑 후보 중 prev에 가장 가까운 라디안 선택
	const float TWO_PI = XM_2PI;
	Vector3 best = eulerRad;
	float minDiff = FLT_MAX;
	for (int i = -1; i <= 1; ++i)
		for (int j = -1; j <= 1; ++j)
			for (int k = -1; k <= 1; ++k)
			{
				Vector3 cand = eulerRad;
				cand.x += i * TWO_PI;
				cand.y += j * TWO_PI;
				cand.z += k * TWO_PI;

				float diff = (cand - prevRad).LengthSquared();
				if (diff < minDiff) 
				{ 
					minDiff = diff; 
					best = cand; 
				}
			}

	// 4) 반환은 UI(deg)
	return {
		XMConvertToDegrees(best.x),
		XMConvertToDegrees(best.y),
		XMConvertToDegrees(best.z)
	};
}

Transform::Transform()
{
}

Transform::Transform(Matrix* InMatrix)
{
	WorldMatrix = *InMatrix;
}

// 부모자식 관계 행렬 계산에 사용하기 위함
Transform Transform::operator*(const Transform& other) const
{
	Matrix m = this->GetWorldMatrix() * other.GetWorldMatrix();

	Transform result;

	result.SetWorldMatrix(m);

	return result;
}

Vector3 Transform::GetForward() const
{
	if (bDirty)
		UpdateWorldMatrix();

	Vector3 forward = Vector3(WorldMatrix._13, WorldMatrix._23, WorldMatrix._33);
	forward.Normalize();
	
	return forward;
}

Vector3 Transform::GetUp() const
{
	if (bDirty)
		UpdateWorldMatrix();

	Vector3 up = Vector3(WorldMatrix._12,WorldMatrix._22,WorldMatrix._32);
	up.Normalize();

	return up;
}

Vector3 Transform::GetRight() const
{
	if (bDirty)
		UpdateWorldMatrix();

	Vector3 right = Vector3(WorldMatrix._11,WorldMatrix._21,WorldMatrix._31);
	right.Normalize();

	return right;
}

/// <summary>
/// WorldMatrix를 내부 값으로 만들어 반환
/// </summary>
const Matrix& Transform::GetWorldMatrix() const
{
	if (bDirty)
		UpdateWorldMatrix();

	return WorldMatrix;
}

void Transform::SetWorldMatrix(const Matrix& matrix)
{
	WorldMatrix = matrix;

	XMVECTOR S, R, T;
	auto xm = XMLoadFloat4x4(&WorldMatrix);

	// NaN/Inf 입력 방지
	if (XMMatrixIsNaN(xm) || XMMatrixIsInfinite(xm)) {
		// PRS는 건드리지 않고 반환 (튀는 것 방지)
		return;
	}

	if (XMMatrixDecompose(&S, &R, &T, XMLoadFloat4x4(&WorldMatrix)))
	{
		// 회전 정규화 + 쿼터니언 연속성(이전 값과 반대 부호 방지)
		R = XMQuaternionNormalize(R);
		Quaternion q; 
		XMStoreFloat4(&q, R);
		if (RotationQuat.Dot(q) < 0.0f) 
			q = -q; // continuity
		
		RotationQuat = q;

		// 스케일 0-division 보호(다음 단계에서 나눗셈이 있을 수 있으니 ε 클램프)
		XMFLOAT3 s; 
		XMStoreFloat3(&s, S);
		const float EPS = 1e-6f;
		auto clamp_eps = [EPS](float v) {
			if (fabsf(v) < EPS) return copysignf(EPS, v == 0.0f ? 1.0f : v);
			return v;
			};

		s.x = clamp_eps(s.x);
		s.y = clamp_eps(s.y);
		s.z = clamp_eps(s.z);
		Scale = { s.x, s.y, s.z };

		XMStoreFloat3(&Position, T);

		// 오일러 변환은 내부에서 반드시 clamp([-1,1]) 하도록
		RotationEuler = ToEulerClosestTo(RotationQuat, RotationEuler);

		bDirty = false; // 성공시에만 클리어
	}

	bDirty = false;
}

void Transform::SetPosition(const Vector3& InValue)
{
	Position = InValue;
	bDirty = true;
}

void Transform::SetScale(const Vector3& InValue)
{
	Scale = InValue;
	bDirty = true;
}

/// <summary>
/// Rotation 회전을 Vector3으로 받을때
/// </summary>
void Transform::SetRotation(const Vector3& InValue)
{
	RotationEuler = InValue;

	Vector3 rad(XMConvertToRadians(InValue.x), XMConvertToRadians(InValue.y), XMConvertToRadians(InValue.z));
	RotationQuat = Quaternion::CreateFromYawPitchRoll(rad.y, rad.x, rad.z);

	bDirty = true;
}

void Transform::SetRotation(const Quaternion& quat)
{
	RotationQuat = quat;
	RotationEuler = ToEulerClosestTo(RotationQuat, RotationEuler);

	bDirty = true;
}

// 현재 이동 값에 값 추가
void Transform::Translate(const Vector3& InDelta)
{
	Position += InDelta;
	bDirty = true;
}

// 현재 회전 값에서 이동 축, 이동 량 받아 반영
void Transform::RotateAxisAngle(const Vector3& Axis, float AngleRad)
{
	Vector3 axis = Axis;
	axis.Normalize();

	Quaternion delta = Quaternion::CreateFromAxisAngle(axis, AngleRad);
	RotationQuat = delta * RotationQuat;

	bDirty = true;
}

// 멤버로 World행렬을 생성
void Transform::UpdateWorldMatrix() const
{
	if (!bDirty)
		return;

	Matrix s = Matrix::CreateScale(Scale);
	Matrix r = Matrix::CreateFromQuaternion(RotationQuat);
	Matrix t = Matrix::CreateTranslation(Position);

	WorldMatrix = s * r * t;

	bDirty = false;
}