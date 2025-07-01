#include "Framework.h"
#include "Transform.h"


Transform::Transform()
{
}

Transform::Transform(Matrix* InMatrix)
{
	WorldMatrix = *InMatrix;
}

Vector3 Transform::GetForward()
{
	if (bDirty)
		UpdateWorldMatrix();

	Vector3 forward = Vector3(WorldMatrix._13, WorldMatrix._23, WorldMatrix._33);
	forward.Normalize();
	
	return forward;
}

Vector3 Transform::GetUp()
{
	if (bDirty)
		UpdateWorldMatrix();

	Vector3 up = Vector3(WorldMatrix._12,WorldMatrix._22,WorldMatrix._32);
	up.Normalize();

	return up;
}

Vector3 Transform::GetRight()
{
	if (bDirty)
		UpdateWorldMatrix();

	Vector3 right = Vector3(WorldMatrix._11,WorldMatrix._21,WorldMatrix._31);
	right.Normalize();

	return right;
}

const Matrix& Transform::GetWorldMatrix()
{
	if (bDirty)
		UpdateWorldMatrix();

	return WorldMatrix;
}

const Vector3& Transform::GetPosition()
{
	return Position;
}

void Transform::SetPosition(const Vector3& InValue)
{
	Position = InValue;
	bDirty = true;
}

const Vector3& Transform::GetScale()
{
	return Scale;
}

void Transform::SetScale(const Vector3& InValue)
{
	Scale = InValue;
	bDirty = true;
}

// 슬라이더용 회전
Vector3 Transform::GetRotation()
{
	return RotationEuler;
}

void Transform::SetRotation(const Vector3& InValue)
{
	RotationEuler = InValue;

	Vector3 rad(XMConvertToRadians(InValue.x), XMConvertToRadians(InValue.y), XMConvertToRadians(InValue.z));
	RotationQuat = Quaternion::CreateFromYawPitchRoll(rad.y, rad.x, rad.z);

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
void Transform::UpdateWorldMatrix()
{
	if (!bDirty)
		return;

	Matrix s = Matrix::CreateScale(Scale);
	Matrix r = Matrix::CreateFromQuaternion(RotationQuat);
	Matrix t = Matrix::CreateTranslation(Position);

	WorldMatrix = s * r * t;

	bDirty = false;
}