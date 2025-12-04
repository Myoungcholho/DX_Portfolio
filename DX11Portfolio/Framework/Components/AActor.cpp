#include "Framework.h"
#include "AActor.h"

static int s_actorID = 0;

/// <summary>
/// 내 루트 밑 내 소유가 아닌 컴포넌트가 섞여 있으면 전부 Detach하고 싶을 때 사용
/// </summary>
static void DetachForeignDescendants(USceneComponent* node, AActor* self)
{
	if (!node) return;
	auto list = node->GetChildren();              // 반드시 '복사본'

	for (auto* c : list)
	{
		if (!c) continue;
		if (c->GetOwner() != self)
		{
			c->Detach(EAttachMode::KeepWorld);    // 손자/증손도 포함해 끊김
		}
		else
		{
			DetachForeignDescendants(c, self);    // 내 소유면 더 내려가서 검사
		}
	}
}

AActor::AActor()
{
	name = "Actor_" + to_string(s_actorID++);
}

void AActor::SetWorld(UWorld* world)
{
	this->world = world;
}

UWorld* AActor::GetWorld() const
{
	return world;
}

void AActor::Tick()
{
	for (auto& comp : components)
		comp->Tick();
}

void AActor::FixedTick(double dt)
{
	for (auto& comp : components)
		comp->FixedTick(dt);
}

void AActor::OnGUI()
{
	for (auto& comp : components)
		comp->OnGUI();
}

void AActor::ClearComponent()
{
	if (root)
	{
		DetachForeignDescendants(root, this);
		if (root->GetParent())
			root->Detach(EAttachMode::KeepWorld);
	}

	// 소유한 컴포넌트들 정리
	auto comps = move(components);
	components.clear();

	for (auto& comp : comps)
		comp->Destroy();		// 외부 시스템 등록 해제, 델리게이트 해제 등

	root = nullptr;
}

/// <summary>
/// Actor 단위의 attach 요청 시 Root 컴포넌트끼리 attach 시도 래퍼
/// </summary>
bool AActor::AttachToActor(AActor* parent, EAttachMode mode)
{
	// 자신에게 Attach
	if (parent == this)
		return false;

	auto myRoot = GetRootComponent();
	if (!myRoot)
		return false;

	USceneComponent* parentRoot = parent ? parent->GetRootComponent() : nullptr;
	
	// parent는 존재하지만 parent의 Root노드가 없다면 return
	if (parent && !parentRoot)
		return false;

	// 월드가 다르다면
	if (parent && parent->GetWorld() != GetWorld())
		return false;

	return myRoot->AttachTo(parentRoot, mode);
}

/// <summary>
/// Actor 단위의 Detach 요청 시 실행 래퍼
/// </summary>
bool AActor::DetachFromActor(EAttachMode mode)
{
	auto myRoot = GetRootComponent();
	if (!myRoot) return false;
	return myRoot->Detach(mode);
}

AActor* AActor::GetParent() const
{
	auto rc = GetRootComponent();						// 내 액터 내부 트리의 '루트 컴포넌트'
	if (!rc) return nullptr;

	USceneComponent* attachParent = rc->GetParent();	// 루트라도, 다른 액터의 컴포넌트에 '붙어' 있을 수 있음
	return attachParent ? attachParent->Owner			// 그 붙어있는 컴포넌트의 소유 액터 = 부모 액터
		: nullptr;										// 안 붙어 있으면 부모 없음
}

/// <summary>
/// Actor 삭제 메서드로, bool 변수 마킹 후 삭제 리스트에 추가
/// </summary>
void AActor::Destroy()
{
	if (bPendingDestroy) return;
	bPendingDestroy = true;

	if (world) 
		world->MarkActorForDestroy(this);
}

Vector3 AActor::GetPosition() const
{
	if (root)
		return root->GetRelativeTransform().GetPosition();
	return actorTransform.GetPosition();
}

void AActor::SetPosition(const Vector3& pos)
{
	if (root)
		root->SetRelativePosition(pos);
	else
		actorTransform.SetPosition(pos);
}

Quaternion AActor::GetRotation() const
{
	if (root)
		return root->GetRelativeRotationQuat();
	return actorTransform.GetRotationQuat();
}

void AActor::SetRotation(const Quaternion& q)
{
	if (root)
		root->SetRelativeRotation(q);
	else
		actorTransform.SetRotation(q);
}