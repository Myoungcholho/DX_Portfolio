#pragma once

using namespace DirectX::SimpleMath;

class Transform
{
public:
	Transform();
	Transform(Matrix* InMatrix);

public:
	Transform operator*(const Transform& other) const;

public:
	Vector3 GetForward();
	Vector3 GetUp();
	Vector3 GetRight();

public:
	const Matrix& GetWorldMatrix();
	void SetWorldMatrix(const Matrix& matrix);

public:
	// Position
	const Vector3& GetPosition() { return Position; }
	void SetPosition(const Vector3& InValue);

	// Scale
	const Vector3& GetScale() { return Scale; }
	void SetScale(const Vector3& InValue);

	// Rotation
	Vector3 GetRotation() { return RotationEuler; }
	void SetRotation(const Vector3& InValue);
	void SetRotation(const Quaternion& quat);

public:
	void Translate(const Vector3& InDelta);
	void RotateAxisAngle(const Vector3& Axis, float AngleRad);

private:
	void UpdateWorldMatrix();

private:
	bool bDirty = false;

private:
	Vector3 Scale = Vector3(1.f, 1.f, 1.f);
	Vector3 Position = Vector3(0.f, 0.f, 0.f);
	Vector3 RotationEuler = Vector3(0.f, 0.f, 0.f); // 슬라이더 UI 용
	Quaternion RotationQuat = Quaternion(0, 0, 0, 1); // 내부 누적 회전용

private:
	Matrix WorldMatrix;
};