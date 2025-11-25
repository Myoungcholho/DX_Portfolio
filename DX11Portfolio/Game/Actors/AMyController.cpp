#include "Pch.h"
#include "AMyController.h"
#include "Gameplay/ClassIDRegister.h"

REGISTER_CLASS(AMyController);

extern "C" void AMyController_Anchor() {}

AMyController::AMyController()
{
	name = "AMyController";
	printf("AMyController createda!\n");
}