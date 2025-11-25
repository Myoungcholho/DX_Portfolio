#include "Framework.h"
#include "AController.h"

AController::AController()
{
	name = "PlayerController";
	controlRotation = Quaternion();
}

void AController::Possess(APawn* InPawn)
{
	if (!InPawn)
		return;

	if (pawn)
	{
		if (pawn == InPawn)
			return;

		UnPossess();
	}

	pawn = InPawn;
	pawn->OnPossessed(this);
}

void AController::UnPossess()
{
	if (pawn)
	{
		pawn->OnUnPossessed();
		pawn = nullptr;
	}
}

void AController::SetControlRotation(const Quaternion& q)
{
	controlRotation = q;
}

void AController::Tick()
{
	if (pawn)
	{
		pawn->ApplyControllerRotation(controlRotation);
	}
}