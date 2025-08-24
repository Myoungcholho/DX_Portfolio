#pragma once
#include "UObject.h"

class UActorComponent : public UObject
{
public:
	class AActor* Owner = nullptr;
	string mName = "ActorComponent";

	virtual void Tick() {}
	virtual void OnAttach(class AActor* inOwner) { Owner = inOwner; }
	virtual void OnRegister() {}
	virtual void OnUnregister() {}
	virtual void OnGUI() {}

	void SetName(string name) { mName = name; }
	AActor* GetOwner() { return Owner; }
};