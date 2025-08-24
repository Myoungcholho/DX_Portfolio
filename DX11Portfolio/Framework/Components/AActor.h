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
	void SetName(const std::string& name) { m_name = name; }
	const string& GetName() { return m_name; }
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
	void Destroy() override;
	virtual void Initialize() {};
	virtual void BeginPlay();
	virtual void Tick();
	virtual void Render();
	virtual void OnGUI();

public:
	//template<typename T>
	//shared_ptr<T> AddComponent(shared_ptr<T> comp) 
	//{
	//	comp->OnAttach(this);			// ActorComponent에서 관리하는 Owner에 등록
	//	Components.push_back(comp);		// 관리 컴포넌트에 등록
	//	return comp;
	//}

	shared_ptr<UActorComponent> AddComponent(shared_ptr<UActorComponent> comp);

	void SetRootComponent(shared_ptr<USceneComponent> comp);
	std::shared_ptr<USceneComponent> GetRootComponent() const { return root; }

protected:
	std::vector<shared_ptr<UActorComponent>> Components;
	std::shared_ptr<USceneComponent> root = nullptr;
	std::string m_name;
	UWorld* m_world = nullptr;
	
public:
	// 리펙토링 대상(1)
	std::shared_ptr<Transform> transform = std::make_shared<Transform>();
	std::shared_ptr<Transform> GetTransform() const;

private:
	bool ContainsComponent(const shared_ptr<UActorComponent>& comp) const;
	void AttachIfSceneComponent(const shared_ptr<UActorComponent>& comp);

private:
	atomic<bool> bPendingDestroy{ false };
};