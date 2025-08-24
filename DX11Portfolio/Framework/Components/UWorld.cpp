#include "Framework.h"
#include "UWorld.h"

void UWorld::Initialize()
{
	// 액터의 초기화는 생성될 때 실행되는게 원칙
	// 여기서는 월드 상태 변수 초기화, 시간이라던지..
	
}

void UWorld::Tick()
{
	auto actors = Actors;

	for (auto& a : Actors)
		if (!a->IsPendingDestroy()) 
			a->Tick();
	
	FlushDestroyed();
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

void UWorld::MarkActorForDestroy(AActor* actor)
{
	if (!actor) 
		return;

	// 삭제 대상에 없는 경우 삭제 대상으로 추가
	if (std::find(PendingDestroy.begin(), PendingDestroy.end(), actor) == PendingDestroy.end())
		PendingDestroy.push_back(actor);
}

void UWorld::FlushDestroyed()
{
	if (PendingDestroy.empty())
		return;

	// vector clear하기 위해 옮겨둠
	auto pending = std::move(PendingDestroy);
	PendingDestroy.clear();

	for (auto* a : pending) 
		RemoveActor(a);
}

void UWorld::RemoveActor(AActor* actor)
{
	if (actor == nullptr)
		return;

	UnregisterFromTick(actor);

	// 컨테이너에 있는지 확인해서 가져옴
	auto it = std::find_if(Actors.begin(), Actors.end(),
		[actor](const AActor* a) { return a == actor; });

	if (it == Actors.end()) 
		return;

	actor->SetWorld(nullptr);
	Actors.erase(it);                   // 참조 0이면 여기서 소멸
	return;
}

void UWorld::UnregisterFromTick(AActor* actor)
{
	// 예: 틱 리스트/월드 이벤트 버스/월드 타이머에서 제거
	// mTickingActors.erase(actor);
	// OnWorldTick.Remove(handleMap[actor]);
	// TimerManager.CancelAllFor(actor);
	// 필요 시 캐시/파티션에서 actor 루트 제거(월드가 직접 관리하는 경우에 한해)
}
