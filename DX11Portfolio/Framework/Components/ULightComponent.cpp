#include "Framework.h"
#include "ULightComponent.h"

void ULightComponent::Tick()
{

}

const LightData& ULightComponent::GetLightData()
{
    m_light.position = m_relativeTransform.GetPosition();
    m_light.direction = m_relativeTransform.GetForward();
    return m_light;
}
