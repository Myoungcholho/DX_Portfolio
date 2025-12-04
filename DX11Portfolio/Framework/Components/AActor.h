#pragma once

#include <memory>
#include <vector>

#include "UActorComponent.h"
#include "USceneComponent.h"
#include "Transform.h"

class UWorld;

class AActor : public UObject
{
public:
	AActor();

public:
	void SetName(const string& InName) { name = InName; }
	const string& GetName() const { return name; }
	void SetWorld(UWorld* world);
	UWorld* GetWorld() const;
	vector<shared_ptr<UActorComponent>>& GetComponents() { return components; }
	bool IsPendingDestroy() const { return bPendingDestroy; }

public:
	// 편의 래퍼, RootComponent 기준으로 처리
	bool AttachToActor(AActor* parent, EAttachMode mode = EAttachMode::KeepWorld);
	bool DetachFromActor(EAttachMode mode = EAttachMode::KeepWorld);
	AActor* GetParent() const;

public:
	virtual void Destroy();
	virtual void Initialize() {};
	virtual void BeginPlay() {};
	virtual void Tick();
	virtual void FixedTick(double dt);
	virtual void OnGUI();

public:
	void ClearComponent();

public:
	template<typename T, typename... Args>
	T* CreateDefaultSubobject(const string& name, Args&&... args)
	{
		auto comp = make_shared<T>(forward<Args>(args)...);
		comp->SetName(name);
		comp->SetOwner(this);

		components.push_back(comp);
		return comp.get();
	}

public:
	//UActorComponent* AddComponent(const shared_ptr<UActorComponent> comp);
	USceneComponent* GetRootComponent() const { return root; }
	void SetRootComponent(USceneComponent* InRoot) { root = InRoot; }

protected:
	vector<shared_ptr<UActorComponent>> components;

	USceneComponent* root = nullptr;							// 향후 사용 버전

	string name;
	UWorld* world = nullptr;

protected:
	Transform actorTransform;				// RootComponent가 없을 때만 사용되는 Transform
public:
	Vector3 GetPosition() const;
	void SetPosition(const Vector3& pos);
	Quaternion GetRotation() const;
	void SetRotation(const Quaternion& q);

private:
	//bool ContainsComponent(const UActorComponent* comp) const;
	//void AttachIfSceneComponent(UActorComponent* comp);

private:
	atomic<bool> bPendingDestroy{ false };
};