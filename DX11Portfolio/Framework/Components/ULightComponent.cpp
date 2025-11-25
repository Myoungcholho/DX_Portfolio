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
    light.position = worldTransform.GetPosition();
    light.direction = worldTransform.GetForward();
    return light;
}
