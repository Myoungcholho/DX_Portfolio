#include "Framework.h"
#include "Camera.h"

using namespace DirectX::SimpleMath;

CCamera::CCamera()
{
	RotationMatrix = Matrix();
	ViewMatrix = Matrix();


	SetRotationMatrix();
	SetViewMatrix();
}

CCamera::~CCamera()
{
}

void CCamera::Tick()
{

}

const Vector3& CCamera::GetPosition()
{
	return Position;
}

void CCamera::SetPosition(float x, float y, float z)
{
	SetPosition(Vector3(x, y, z));
}

void CCamera::SetPosition(const Vector3& InPosition)
{
	Position = InPosition;

	SetViewMatrix();
}

Vector3 CCamera::GetRotation()
{
	Vector3 r;
	r.x = XMConvertToDegrees(Rotation.x);
	r.y = XMConvertToDegrees(Rotation.y);
	r.z = XMConvertToDegrees(Rotation.z);

	return r;
}

void CCamera::SetRotation(float x, float y, float z)
{
	SetRotation(Vector3(x, y, z));
}

void CCamera::SetRotation(const Vector3& InRotation)
{
	Rotation.x = XMConvertToRadians(InRotation.x);
	Rotation.y = XMConvertToRadians(InRotation.y);
	Rotation.z = XMConvertToRadians(InRotation.z);

	SetRotationMatrix();
	SetViewMatrix();
}

void CCamera::SetViewMatrix()
{
	//ViewMatrix = Matrix::CreateLookAt(Position, (Position + Forward), Up);
	//ViewMatrix = FMath::CreateLookAtLH(Position, (Position + Forward), Up);
	ViewMatrix = DirectX::XMMatrixLookAtLH(Position, (Position + Forward), Up);

	float width = D3D::GetDesc().Width;
	float height = D3D::GetDesc().Height;

	Projection = DirectX::XMMatrixPerspectiveFovLH
	(
		DirectX::XM_PI * 0.25f,
		width / height,
		0.1f,
		1000.0f
	);
}

void CCamera::SetRotationMatrix()
{
	Matrix x, y, z;
	x = Matrix::CreateRotationX(Rotation.x);
	y = Matrix::CreateRotationY(Rotation.y);
	z = Matrix::CreateRotationZ(Rotation.z);

	RotationMatrix = x * y * z;

	Forward = Vector3::TransformNormal(MathHelper::Forward, RotationMatrix);
	Up = Vector3::TransformNormal(MathHelper::Up, RotationMatrix);
	Right = Vector3::TransformNormal(MathHelper::Right, RotationMatrix);
}