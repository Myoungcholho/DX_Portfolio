#include "Framework.h"
#include "UPrimitiveComponent.h"

void UPrimitiveComponent::RefreshConstantsCPU()
{
	m_meshConstsCPU.World = m_worldTransform.GetWorldMatrix().Transpose();
	m_meshConstsCPU.InvTranspose = m_meshConstsCPU.World;
	m_meshConstsCPU.InvTranspose.Translation(Vector3(0.0f));
	m_meshConstsCPU.InvTranspose = m_meshConstsCPU.InvTranspose.Transpose().Invert();

	//D3D11Utils::UpdateBuffer(device, context, m_meshConstsCPU, m_meshConstsGPU);
	// 왜 주석처리했냐면
	// 1. GetProxy()를 매프레임 얻을때 Component의 최신 데이터를 복사해가기 때문
	// 2. 그리고 D3D의 업데이트는 실제 렌더링전에 Update를 명시호출하기 때문
	// 3. 그래서 값 변경에 매번 업데이트 코드를 뺀것.
}