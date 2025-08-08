#include "Framework.h"
#include "UWorld.h"

void UWorld::Initialize()
{
	// 액터의 초기화는 생성될 때 실행되는게 원칙
	// 여기서는 월드 상태 변수 초기화, 시간이라던지..
	
}

void UWorld::Tick()
{
	for (auto actor : Actors)
		actor->Tick();
}

void UWorld::Render()
{
	vector<shared_ptr<URenderProxy>> proxies;
	vector<LightData> lights;


	for (auto actor : Actors)
	{
		for (auto comp : actor->GetComponents())
		{
			// 렌더 프록시 수집
			if (auto prim = dynamic_cast<UPrimitiveComponent*>(comp.get()))
			{
				auto proxy = prim->GetRenderProxy();

				proxies.push_back(std::move(proxy));
			}

			// 라이트 수집
			if(auto light = dynamic_cast<ULightComponent*>(comp.get()))
			{
				lights.push_back(light->GetLightData()); // 위치, 색, 방향 등
			}

		}
	}

	if(m_renderManager)
		m_renderManager->EnqueueProxies(std::move(proxies),lights);
}

void UWorld::Destroy()
{
	for (auto actor : Actors)
		actor->Destroy();
}

void UWorld::OnGUI()
{
	for (auto actor : Actors)
		actor->OnGUI();
}

///////////////////////////////////////////////////////////////////////////////

// 월드에 올라갈 액터들 데이터 준비가 완료되면 시작
void UWorld::StartAllActors()
{
	for (auto actor : Actors)
		actor->Initialize();

	for (auto actor : Actors)
		actor->BeginPlay();
}

