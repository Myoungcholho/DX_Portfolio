#include "Framework.h"
#include "UGameInstance.h"

UGameInstance::UGameInstance()
{
	// 아직은 no
}

void UGameInstance::Init()
{
	m_world = make_unique<UWorld>();
	m_renderManager = make_unique<URenderManager>();

	m_world->Initialize();
	m_renderManager->Init();
}

void UGameInstance::Tick()
{
	if (m_world == nullptr)
		return;

	m_world->Tick();
}

void UGameInstance::Render()
{
	if (m_world == nullptr && m_renderManager == nullptr)
		return;

	m_renderManager->Render(m_world.get());

	// GameInstance가 하기에는 부적합한 작업들
	//Vector3 eye = CContext::Get()->GetCamera()->GetPosition();
	//Matrix view = CContext::Get()->GetViewMatrix();
	//Matrix proj = CContext::Get()->GetProjectionMatrix();

	//m_world->Render();									// 큐 구성

	//m_renderer->UpdateGlobalConstants(eye, view, proj);
	//m_renderer->RenderFrame(m_world->GetRenderQueue());

	// 거울 렌더링 (reflect 버퍼)
	//m_renderer->RenderMirror(m_world->GetRenderQueue(), eye, view, proj); // 여기에 카메라 정보 전달

	// 후처리 (렌더러 안에서 하지 말고 여기서 분리)
	//m_renderer->PostProcess();
	//m_renderer->Present();
}

void UGameInstance::Destroy()
{

}

void UGameInstance::OnGUI()
{
	if (m_world)
		m_world->OnGUI();				// 컴포넌트 개인 값 수정은 여기서
	
	if(m_renderManager)
		m_renderManager->OnGUI();		// 전역 렌더러 수정은 여기서
}
