#include "Pch.h"
#include "ALight.h"

ALight::ALight()
{
	m_lightComponent = make_shared<ULightComponent>();
	root = m_lightComponent;
	AddComponent(m_lightComponent);
}

void ALight::Initialize()
{
	m_lightComponent->SetRadiance(Vector3(5.0f));
	m_lightComponent->SetFalloff(0.0f,20.0f);
	m_lightComponent->SetLightType((int)LightType::Point);
}

shared_ptr<ULightComponent> ALight::GetLightComponent()
{
	return m_lightComponent;
}