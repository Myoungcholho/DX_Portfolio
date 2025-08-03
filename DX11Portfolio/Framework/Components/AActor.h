#pragma once

#include <memory>
#include <vector>

#include "UActorComponent.h"
#include "USceneComponent.h"
#include "Transform.h"

class UWorld;

class AActor : public UObject
{
protected:
	std::vector<shared_ptr<UActorComponent>> Components;
	std::shared_ptr<USceneComponent> root = nullptr;
	UWorld* m_world = nullptr;

public:
	std::shared_ptr<Transform> transform = std::make_shared<Transform>();

public:
	void Destroy() override;

public:
	virtual void Initialize() {};
	virtual void BeginPlay();
	virtual void Tick();
	virtual void Render();
	virtual void OnGUI();

public:
	vector<shared_ptr<UActorComponent>>& GetComponents() { return Components; }

public:
	template<typename T>
	shared_ptr<T> AddComponent(shared_ptr<T> comp) 
	{
		comp->OnAttach(this);			// ActorComponent에서 관리하는 Owner에 등록
		Components.push_back(comp);		// 관리 컴포넌트에 등록
		return comp;
	}

	shared_ptr<UActorComponent> AddComponent(shared_ptr<UActorComponent> comp)
	{
		comp->OnAttach(this);
		Components.push_back(comp);
		return comp;
	}

	void SetRootComponent(shared_ptr<USceneComponent> comp);
	std::shared_ptr<USceneComponent> GetRootComponent() const { return root; }
	std::shared_ptr<Transform> GetTransform() const;

	void SetWorld(UWorld* world);
	UWorld* GetWorld() const;
};