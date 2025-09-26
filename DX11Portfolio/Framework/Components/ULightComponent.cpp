#include "Framework.h"
#include "ULightComponent.h"

ULightComponent::ULightComponent()
{
    mName = "LightComponent";
}

void ULightComponent::Tick()
{
    
}

const LightData& ULightComponent::GetLightData()
{
    m_light.position = worldTransform.GetPosition();
    m_light.direction = worldTransform.GetForward();
    return m_light;
}
