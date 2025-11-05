#include "Framework.h"
#include "ACharacter.h"

ACharacter::ACharacter()
{

}

void ACharacter::Tick()
{
	double dt = CTimer::Get()->GetDeltaTime();

	ApplyMovement(dt);
}

void ACharacter::AddMovementInput(const Vector3& dir, float sclae)
{

}

void ACharacter::ApplyMovement(double dt)
{

}

void ACharacter::ApplyGravity(double dt)
{

}

void ACharacter::UpdateRotationByController(double dt)
{

}
