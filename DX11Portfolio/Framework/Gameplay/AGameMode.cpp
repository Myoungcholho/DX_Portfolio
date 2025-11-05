#include "Framework.h"
#include "AGameMode.h"
#include "Gameplay/ClassIDRegister.h"

REGISTER_CLASS(AGameMode);

AGameMode::AGameMode()
{
    m_name = "AGameMode";
}

void AGameMode::StartPlay()
{
    PlayerPawn = static_cast<APawn*>(ClassID::Create(PawnClassName, world));
    PlayerController = static_cast<APlayerController*>(ClassID::Create(ControllerClassName, world));

    if (PlayerController && PlayerPawn)
        PlayerController->Possess(PlayerPawn);
}

void AGameMode::Tick()
{

}

void AGameMode::SetDefaultPawnClass(const string& pawnName)
{
    PawnClassName = pawnName;
}

void AGameMode::SetDefaultControllerClass(const string& controllerName)
{
    ControllerClassName = controllerName;
}