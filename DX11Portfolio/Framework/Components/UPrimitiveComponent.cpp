#include "Framework.h"
#include "UPrimitiveComponent.h"

void UPrimitiveComponent::RefreshConstantsCPU()
{
	m_meshConstsCPU.World = m_worldTransform.GetWorldMatrix().Transpose();
	m_meshConstsCPU.InvTranspose = m_meshConstsCPU.World;
	m_meshConstsCPU.InvTranspose.Translation(Vector3(0.0f));
	m_meshConstsCPU.InvTranspose = m_meshConstsCPU.InvTranspose.Transpose().Invert();

	//D3D11Utils::UpdateBuffer(device, context, m_meshConstsCPU, m_meshConstsGPU);
}