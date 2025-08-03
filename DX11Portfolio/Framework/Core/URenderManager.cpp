#include "Framework.h"
#include "URenderManager.h"

URenderManager::URenderManager()
{

}

void URenderManager::Init()
{
	m_renderer = make_unique<URenderer>();
	m_renderer->Init();
}

void URenderManager::Render(UWorld* world)
{
	if (!world || !m_renderer) return;

	// 1. 큐 구성
	world->Render();

	// 2. 전역 상수 설정
	Vector3 eye = CContext::Get()->GetCamera()->GetPosition();
	Matrix view = CContext::Get()->GetViewMatrix();
	Matrix proj = CContext::Get()->GetProjectionMatrix();

	m_renderer->UpdateGlobalConstants(eye, view, proj);

	// 3. Render
	m_renderer->RenderFrame(world->GetRenderQueue());

	m_renderer->RenderPostProcess();

	m_renderer->Present();
}

void URenderManager::OnGUI()
{
	// 전역 렌더링 값 수정
	if (m_renderer)
		m_renderer->OnGUI();
}