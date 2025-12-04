#include "Pch.h"
#include "ALight.h"

ALight::ALight()
{
	/*lightComponent = make_shared<ULightComponent>();
	SetRootComponent(lightComponent);
	AddComponent(lightComponent);*/

	USceneComponent* sc = CreateDefaultSubobject<USceneComponent>("root");
	SetRootComponent(sc);

	lightComponent = CreateDefaultSubobject<ULightComponent>("light");
	lightComponent->AttachTo(sc,EAttachMode::KeepRelative);
}

void ALight::Initialize()
{
	lightComponent->SetRadiance(Vector3(3.0f));
	lightComponent->SetFalloff(0.0f,20.0f);
}