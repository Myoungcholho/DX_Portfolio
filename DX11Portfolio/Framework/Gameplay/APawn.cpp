#include "Framework.h"
#include "APawn.h"

APawn::APawn()
{
	SetName("Pawn");
}

void APawn::Tick()
{
	AActor::Tick();

	double dt = CTimer::Get()->GetDeltaTime();

	// --- 이동 처리 ---
	if (PendingMovementInput.Length() > 0.0f)
	{
		PendingMovementInput.Normalize();
		Vector3 moveDir = PendingMovementInput;

		if (Controller != nullptr)
		{
			const Quaternion& ctrlRot = GetRotation();
			moveDir = MathHelper::RotateVectorByQuaternion(moveDir, ctrlRot);
		}

		Vector3 pos = GetPosition();
		pos += moveDir * MoveSpeed * (float)dt;
		SetPosition(pos);
	}

	PendingMovementInput = Vector3(0, 0, 0);
}

void APawn::Destroy()
{
	if (Controller != nullptr)
		Controller->UnPossess();

	AActor::Destroy();
}


void APawn::AddMovementInput(const Vector3& direction, float scale)
{
	PendingMovementInput += direction * scale;
}

void APawn::AddRotationInput(const Vector3& eulerDelta)
{

}

void APawn::ApplyControllerRotation(const Quaternion& ctrlRot)
{
	Vector3 euler = ctrlRot.ToEuler(); // 라디안 단위
	float yaw = bUseControllerRotationYaw ? euler.y : 0.0f;
	float pitch = bUseControllerRotationPitch ? euler.x : 0.0f;
	float roll = bUseControllerRotationRoll ? euler.z : 0.0f;

	// 선택적으로 회전 결합
	Quaternion finalRot = Quaternion::CreateFromYawPitchRoll(yaw, pitch, roll);

	// 루트모션, AI 회전, 블렌드 등 고려할 수 있는 포인트
	SetRotation(finalRot);
}

void APawn::OnPossessed(AController* newController)
{
	Controller = newController;
}

void APawn::OnUnPossessed()
{
	Controller = nullptr;
}