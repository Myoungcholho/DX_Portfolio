#pragma once
#include "UObject.h"

class UActorComponent : public UObject
{
public:
	class AActor* Owner = nullptr;
	string mName = "ActorComponent";

	virtual void Tick() {}
	virtual void FixedTick(double dt) {}
	virtual void OnAttach(class AActor* inOwner) { Owner = inOwner; }
	virtual void OnRegister() {}
	virtual void OnUnregister() {}
	virtual void OnGUI() {}
	virtual void Destroy() {}

	void SetName(string name) { mName = name; }

	AActor* GetOwner() { return Owner; }             // ºñ-const °´Ã¼¿ë
	const AActor* GetOwner() const { return Owner; } // const °´Ã¼¿ë
};