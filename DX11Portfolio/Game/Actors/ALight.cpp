#include "Pch.h"
#include "ALight.h"

ALight::ALight()
{
	lightComponent = make_shared<ULightComponent>();
	SetRootComponent(lightComponent);
	AddComponent(lightComponent);
}

void ALight::Initialize()
{
	lightComponent->SetRadiance(Vector3(3.0f));
	lightComponent->SetFalloff(0.0f,20.0f);
}

shared_ptr<ULightComponent> ALight::GetLightComponent()
{
	return lightComponent;
}