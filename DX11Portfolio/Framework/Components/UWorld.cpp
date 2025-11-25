#include "Framework.h"
#include "UWorld.h"

void UWorld::Initialize()
{
	// 액터의 초기화는 생성될 때 실행되는게 원칙
	// 추후 월드 상태 변수 초기화, 시간 설정
}

void UWorld::Tick()
{
	// Editor가 보낸 변경사항 처리
	FEditorCommandQueue::ProcessAll();

	// 이번 프레임에 순회할 Actor 스냅샷
	vector<AActor*> snapshot;
	snapshot.reserve(Actors.size());

	for (auto& uptr : Actors)
		snapshot.push_back(uptr.get());

	for (AActor* a : snapshot)
	{
		if (a && !a->IsPendingDestroy())
			a->Tick();
	}
}

void UWorld::FixedTick(double fixedDt)
{
	vector<AActor*> snapshot;
	snapshot.reserve(Actors.size());
	for (auto& uptr : Actors)
		snapshot.push_back(uptr.get());

	for (AActor* a : snapshot)
	{
		if (a && !a->IsPendingDestroy())
			a->FixedTick(fixedDt);
	}
}

void UWorld::ProxySnapshot()
{
	vector<shared_ptr<URenderProxy>> proxies;
	vector<LightData> lights;

	for (auto const& actor : Actors)
	{
		for (auto comp : actor->GetComponents())
		{
			// 렌더 프록시 수집
			if (auto prim = dynamic_cast<UPrimitiveComponent*>(comp.get()))
			{
				auto proxy = prim->GetRenderProxy();

				proxies.push_back(move(proxy));
			}

			// 라이트 수집
			if(auto light = dynamic_cast<ULightComponent*>(comp.get()))
			{
				lights.push_back(light->GetLightData()); // 위치, 색, 방향 등
			}

		}
	}

	if(RenderManager)
		RenderManager->EnqueueProxies(move(proxies),move(lights));
}

// World가 명시적으로 파괴될 때 호출
void UWorld::Destroy()
{
	// unique_ptr 값 복사 금지라서 아래는 불가능함
	//for (auto actor : Actors)
	//	actor->Destroy();

	// 1. 남은 액터들 마킹
	for (auto const& actor : Actors)
		actor->Destroy();

	// 2. 마지막 배치
	FlushDestroyed();
}

void UWorld::OnGUI()
{
	for (auto const& actor : Actors)
		actor->OnGUI();
}

///////////////////////////////////////////////////////////////////////////////

// 월드에 올라갈 액터들 데이터 준비가 완료 후 호출
void UWorld::StartAllActors()
{
	for (auto const& actor : Actors)
		actor->Initialize();

	for (auto const& actor : Actors)
		actor->BeginPlay();
}

vector<AActor*> UWorld::GetActorsOf() const
{
	vector<AActor*> out;
	out.reserve(Actors.size());
	for (auto const& actor : Actors)
		out.push_back(actor.get());

	return out;
}

// Actor가 본인을 인자로 전달해 UWorld의 파괴 관리 컨테이너에 등록
void UWorld::MarkActorForDestroy(AActor* actor)
{
	if (!actor) 
		return;

	// 삭제 리스트에 없으면 추가, 있다면 중복 추가 방지
	if (find(PendingDestroy.begin(), PendingDestroy.end(), actor) == PendingDestroy.end())
		PendingDestroy.push_back(actor);
}

// 삭제 마크(PendingKill)가 된 객체가 있다면, 삭제를 실행
// Tick에서 다른 액터가 참조중인데 바로 삭제되면 문제가 되므로 지연 삭제
void UWorld::FlushDestroyed()
{
	if (PendingDestroy.empty())
		return;

	// 프레임 끝에 파괴를 한 번에 적용한다 의도로
	vector<AActor*> pending = move(PendingDestroy);
	PendingDestroy.clear();

	for (auto* a : pending) 
		RemoveActor(a);
}

// 인자로 받은 actor를 UWorld의 관리를 제거 및 메모리 해제
void UWorld::RemoveActor(AActor* actor)
{
	if (actor == nullptr)
		return;

	// 액터 관리 컨테이너에 있는지 확인해서 가져옴
	auto it = find_if(Actors.begin(), Actors.end(),
		[actor](const unique_ptr<AActor>& p) { return p.get() == actor; });

	if (it == Actors.end()) 
		return;

	actor->ClearComponent();

	actor->SetWorld(nullptr);
	Actors.erase(it);
	return;
}
