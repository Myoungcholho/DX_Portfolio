#include "Framework.h"
#include "UPrimitiveComponent.h"

void UPrimitiveComponent::RefreshConstantsCPU()
{
	meshConstsCPU.World = worldTransform.GetWorldMatrix().Transpose();
	meshConstsCPU.InvTranspose = meshConstsCPU.World;
	meshConstsCPU.InvTranspose.Translation(Vector3(0.0f));
	meshConstsCPU.InvTranspose = meshConstsCPU.InvTranspose.Transpose().Invert();

	//GPU 업데이트는 렌더링 직전에 Proxy에서 수행하므로 여기선 생략
	//D3D11Utils::UpdateBuffer(device, context, m_meshConstsCPU, m_meshConstsGPU);
}