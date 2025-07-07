#pragma once

#include <directxtk/SimpleMath.h>

using DirectX::SimpleMath::Matrix;
using DirectX::SimpleMath::Vector3;

class CCamera
{
public:
	CCamera();
	~CCamera();

	void Tick();

public:
	const Vector3& GetPosition();
	void SetPosition(float x, float y, float z);
	void SetPosition(const Vector3& InPosition);

	Vector3 GetRotation();
	void SetRotation(float x, float y, float z);
	void SetRotation(const Vector3& InRotation);

	Vector3 GetForward() { return Forward; }
	Vector3 GetUp() { return Up; }
	Vector3 GetRight() { return Right; }

	Matrix& GetViewMatrix() { return ViewMatrix; }
	Matrix& GetProjectionMatrix() { return Projection; }


	float GetMoveSpeed() { return MoveSpeed; }
	float GetRotationSpeed() { return RotationSpeed; }

public:
	void SetMoveSpeed(float InValue) { MoveSpeed = InValue; }
	void SetRotationSpeed(float InValue) { RotationSpeed = InValue; }

private:
	void SetViewMatrix();
	void SetRotationMatrix();

private:
	Vector3 Position = Vector3(0, 0, 0);
	Vector3 Rotation = Vector3(0, 0, 0);

	Vector3 Forward = Vector3(0, 0, 1);
	Vector3 Up = Vector3(0, 1, 0);
	Vector3 Right = Vector3(1, 0, 0);

private:
	float MoveSpeed = 10.0f;
	float RotationSpeed = 20.f;

private:
	Matrix ViewMatrix;
	Matrix RotationMatrix;

	Matrix Projection;
};