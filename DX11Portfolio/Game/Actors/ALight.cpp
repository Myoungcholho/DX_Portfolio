#include "Pch.h"
#include "ALight.h"

ALight::ALight()
{
	//m_name = "PointLight";
	m_lightComponent = make_shared<ULightComponent>();
	SetRootComponent(m_lightComponent);
	AddComponent(m_lightComponent);
}

void ALight::Initialize()
{
	m_lightComponent->SetRadiance(Vector3(3.0f));
	m_lightComponent->SetFalloff(0.0f,20.0f);
}

shared_ptr<ULightComponent> ALight::GetLightComponent()
{
	return m_lightComponent;
}