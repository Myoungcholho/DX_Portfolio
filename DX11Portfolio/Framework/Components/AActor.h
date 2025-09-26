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
	void SetName(const string& name) { m_name = name; }
	const string& GetName() const { return m_name; }
	void SetWorld(UWorld* world);
	UWorld* GetWorld() const;
	vector<shared_ptr<UActorComponent>>& GetComponents() { return Components; }
	bool IsPendingDestroy() const { return bPendingDestroy; }

public:
	// 편의 래퍼, RootComponent 기준으로 처리
	bool AttachToActor(AActor* parent, EAttachMode mode = EAttachMode::KeepWorld);
	bool DetachFromActor(EAttachMode mode = EAttachMode::KeepWorld);
	AActor* GetParent() const;

public:
	void Destroy();
	virtual void Initialize() {};
	virtual void BeginPlay() {};
	virtual void Tick();
	virtual void FixedTick(double dt);
	virtual void OnGUI();

public:
	shared_ptr<UActorComponent> AddComponent(shared_ptr<UActorComponent> comp);

	void SetRootComponent(shared_ptr<USceneComponent> comp);
	shared_ptr<USceneComponent> GetRootComponent() const { return root; }


protected:
	vector<shared_ptr<UActorComponent>> Components;
	shared_ptr<USceneComponent> root = nullptr;
	string m_name;
	UWorld* m_world = nullptr;

private:
	bool ContainsComponent(const shared_ptr<UActorComponent>& comp) const;
	void AttachIfSceneComponent(const shared_ptr<UActorComponent>& comp);

private:
	atomic<bool> bPendingDestroy{ false };
};