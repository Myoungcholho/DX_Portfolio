#include "Framework.h"
#include "AActor.h"

void AActor::SetRootComponent(shared_ptr<USceneComponent> comp)
{
	root = comp;
	AddComponent(comp);
}

std::shared_ptr<Transform> AActor::GetTransform() const
{
	return nullptr;
}

void AActor::SetWorld(UWorld* world)
{
	m_world = world;
}

UWorld* AActor::GetWorld() const
{
	return m_world;
}

void AActor::BeginPlay()
{

}

void AActor::Tick()
{
	for (auto comp : Components)
		comp->Tick();
}

void AActor::Render()
{
	// 렌더링 가능한 컴포넌트의 Type을 보고 Queue에 넣기
	/*for (auto comp : Components)
	{
		if (auto prim = dynamic_cast<UPrimitiveComponent*>(comp.get()))
			GetWorld()->GetRenderQueue()->Add(prim);
	}*/

	// 이제 UWorld에서 직접 얻어서 돌림
}

void AActor::OnGUI()
{
	for (auto comp : Components)
		comp->OnGUI();
}

void AActor::Destroy()
{
	for (auto comp : Components)
		comp->Destroy();
}