#include "Framework.h"
#include "UGameInstance.h"

UGameInstance::UGameInstance()
{
	// 아직은 no
}

void UGameInstance::Init()
{
	world = make_unique<UWorld>();
	renderManager = make_unique<URenderManager>();

	world->SetRenderManager(renderManager.get());
	world->Initialize();
	renderManager->Init();
}

/**/
void UGameInstance::Tick()
{
	//PRO_BEGIN(L"Game_Tick");
	
	const double dt = CTimer::Get()->GetDeltaTime();
	fixedAcc += dt;

	// 가변 업데이트
	if (world)
		world->Tick();

	// 고정 업데이트
	while (fixedAcc >= fixedDt)
	{
		if (world)
			world->FixedTick(fixedDt);
		fixedAcc -= fixedDt;
	}

	// 삭제될 액터들 삭제
	if(world)
		world->FlushDestroyed();

	//PRO_END(L"Game_Tick");
}

void UGameInstance::Render()
{
	if (world == nullptr && renderManager == nullptr)
		return;

	PRO_BEGIN(L"Game_ProxySnapshot");
	
	if (world)
		world->ProxySnapshot();					// 프록시 생성 → 렌더 매니저에 제출

	PRO_END(L"Game_ProxySnapshot");

}

void UGameInstance::Shotdown()
{
	//if (m_renderManager)
		//m_renderManager->Stop();

	if (world)
		world->Destroy();
}

void UGameInstance::OnGUI()
{
	if (world)
		world->OnGUI();					// 컴포넌트 개인 값 수정은 여기서
	
	if(renderManager)
		renderManager->OnGUI();			// 전역 렌더러 수정은 여기서
}
