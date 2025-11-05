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
	Vector3 GetForward() const;
	Vector3 GetUp() const;
	Vector3 GetRight() const;

public:
	const Matrix& GetWorldMatrix() const;
	void SetWorldMatrix(const Matrix& matrix);

public:
	// Position
	const Vector3& GetPosition() const { return Position; }
	void SetPosition(const Vector3& InValue);

	// Scale
	const Vector3& GetScale() const { return Scale; }
	void SetScale(const Vector3& InValue);

	// Rotation
	Vector3 GetRotation() const { return RotationEuler; }
	void SetRotation(const Vector3& InValue);
	void SetRotation(const Quaternion& quat);
	const Quaternion& GetRotationQuat() const { return RotationQuat; }

public:
	void Translate(const Vector3& InDelta);
	void RotateAxisAngle(const Vector3& Axis, float AngleRad);

private:
	void UpdateWorldMatrix() const;


private:
	Vector3 Scale = Vector3(1.f, 1.f, 1.f);
	Vector3 Position = Vector3(0.f, 0.f, 0.f);
	Vector3 RotationEuler = Vector3(0.f, 0.f, 0.f);		// 슬라이더 UI 용
	Quaternion RotationQuat = Quaternion(0, 0, 0, 1);	// 내부 누적 회전용
private:
	mutable bool bDirty = false;

private:
	mutable Matrix WorldMatrix;
};