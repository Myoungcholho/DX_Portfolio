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
	m_renderQueue.Clear();

	// 액터들을 RenderQueue에 등록
	for (auto actor : Actors)
		actor->Render();
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

void UWorld::RegisterLight(ULightComponent* light)
{
	LightComponents.push_back(light);
}

void UWorld::UnregisterLight(ULightComponent* light)
{
	erase(LightComponents, light);
}

const vector<ULightComponent*>& UWorld::GetLights() const
{
	return LightComponents;
}