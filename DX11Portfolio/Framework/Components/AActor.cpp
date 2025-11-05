#include "Framework.h"
#include "AActor.h"

static int s_actorID = 0;

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
	m_name = "Actor_" + to_string(s_actorID++);
}

shared_ptr<UActorComponent> AActor::AddComponent(shared_ptr<UActorComponent> comp)
{
	if (comp == nullptr)
		return nullptr;

	// Owner 설정
	comp->OnAttach(this);

	if(!ContainsComponent(comp))
		Components.push_back(comp);

	// 루트 컴포넌트가 있다면 루트 컴포넌트에 어태치
	AttachIfSceneComponent(comp);

	return comp;
}

void AActor::SetRootComponent(shared_ptr<USceneComponent> comp)
{
	auto oldRoot = root;
	root = move(comp);					// 일반 대입 사용하면 refcount가 1증가함

	if (root)
	{
		root->OnAttach(this);

		// 이전 Transform을 Root로 이전
		root->SetRelativeTransform(actorTransform);
		root->UpdateWorldTransformRecursive();

		// 기존 actorTransform은 더 이상 직접 사용하지 않음
		actorTransform = Transform();

		if (!ContainsComponent(root))
			Components.push_back(root);
	}
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
	// auto comp 쓰고 있었는데 이는 포인터 주소를 복사해 약간의 오버헤드가 있음.
	// const auto& 로 변경
	for (auto& comp : Components)
		comp->Tick();
}

void AActor::FixedTick(double dt)
{
	for (auto& comp : Components)
		comp->FixedTick(dt);
}

void AActor::OnGUI()
{
	for (auto& comp : Components)
		comp->OnGUI();
}

void AActor::ClearComponent()
{
	if (root)
	{
		DetachForeignDescendants(root.get(), this);
		if (root->GetParent())
			root->Detach(EAttachMode::KeepWorld);
	}

	// 소유한 컴포넌트들 정리
	auto comps = move(Components);
	Components.clear();

	for (auto& comp : comps)
		comp->Destroy();		// 외부 시스템 등록 해제, 델리게이트 해제 등

	root.reset();
}

bool AActor::AttachToActor(AActor* parent, EAttachMode mode)
{
	// 자신에게 Attach
	if (parent == this)
		return false;

	auto myRoot = GetRootComponent();
	if (!myRoot)
		return false;

	USceneComponent* parentRoot = parent ? parent->GetRootComponent().get() : nullptr;
	
	// parent는 존재하지만 parent의 Root노드가 없다면 return
	if (parent && !parentRoot)
		return false;

	// 월드가 다르다면
	if (parent && parent->GetWorld() != GetWorld())
		return false;

	return myRoot->AttachTo(parentRoot, mode);
}

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
/// Actor 삭제 메서드
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

/// <summary>
/// vector<..> Components에 가지고 있는지 확인
/// </summary>
bool AActor::ContainsComponent(const shared_ptr<UActorComponent>& comp) const
{
	return find(Components.begin(), Components.end(), comp) != Components.end();
}

/// <summary>
/// 루트에 씬 컴포넌트 어태치
/// </summary>
void AActor::AttachIfSceneComponent(const shared_ptr<UActorComponent>& comp)
{	
	shared_ptr<USceneComponent> sc = dynamic_pointer_cast<USceneComponent>(comp);

	if (sc == nullptr)
		return;
	if (root == nullptr)
		return;
	if (sc == root)
		return;

	sc->AttachTo(root.get(), EAttachMode::KeepWorld);
}