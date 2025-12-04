#include "Framework.h"
#include "APlayerController.h"
#include "Gameplay/ClassIDRegister.h"

REGISTER_CLASS(APlayerController);

APlayerController::APlayerController()
{
	
}

void APlayerController::Tick()
{
	AController::Tick();

	double dt = CTimer::Get()->GetDeltaTime();
	HandleInput(dt);
}

void APlayerController::HandleInput(double dt)
{
	CKeyboard* key = CKeyboard::Get();
	CMouse* mouse = CMouse::Get();

	if (key == nullptr || mouse == nullptr)
		return;

	if (pawn == nullptr)
		return;

	// --- 마우스 입력에 따라 시점 회전 업데이트 ---
	if (mouse->Press(EMouseButton::Left))
	{
		Vector3 mouseDelta = mouse->GetMoveDelta();
		yaw += mouseDelta.x * mouseSensitivity;
		pitch += mouseDelta.y * mouseSensitivity;

		// 쿼터니언 회전 생성 (Yaw→Pitch 순서)
		Quaternion qYaw = Quaternion::CreateFromAxisAngle(Vector3(0, 1, 0), XMConvertToRadians(yaw));
		Quaternion qPitch = Quaternion::CreateFromAxisAngle(Vector3(-1, 0, 0), XMConvertToRadians(pitch));
		Quaternion finalRot = qYaw * qPitch;

		SetControlRotation(finalRot);
	}

	// --- 키보드 이동 입력 ---
	if (mouse->Press(EMouseButton::Left))
	{
		if (key->Press('W'))
			pawn->AddMovementInput(Vector3(0, 0, 1));
		else if (key->Press('S'))
			pawn->AddMovementInput(Vector3(0, 0, -1));

		if (key->Press('A'))
			pawn->AddMovementInput(Vector3(-1, 0, 0));
		else if (key->Press('D'))
			pawn->AddMovementInput(Vector3(1, 0, 0));
	}
}

void APlayerController::UpdateViewRotation(double dt)
{
	// 회전은 나중에 Camera 클래스 컴포넌트화 이후 처리
}