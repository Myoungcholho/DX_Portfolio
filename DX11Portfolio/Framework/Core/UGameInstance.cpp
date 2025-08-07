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

	m_world->SetRenderManager(m_renderManager.get());
	m_world->Initialize();
	m_renderManager->Init();
}

void UGameInstance::Tick()
{
	//PRO_BEGIN(L"Game_Tick");
	
	if (m_world)
		m_world->Tick();

	//PRO_END(L"Game_Tick");
}

void UGameInstance::Render()
{
	if (m_world == nullptr && m_renderManager == nullptr)
		return;

	//PRO_BEGIN(L"Game_Render");
	
	if (m_world)
		m_world->Render();					// 프록시 생성 → 렌더 매니저에 제출

	//PRO_END(L"Game_Render");

}

void UGameInstance::Destroy()
{
	if (m_world)
		m_world->Destroy();
}

void UGameInstance::OnGUI()
{
	if (m_world)
		m_world->OnGUI();					// 컴포넌트 개인 값 수정은 여기서
	
	if(m_renderManager)
		m_renderManager->OnGUI();			// 전역 렌더러 수정은 여기서
}
