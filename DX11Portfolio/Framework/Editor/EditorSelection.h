#pragma once

class AActor;
class UActorComponent;

class EditorSelection
{
public:
	static void SetActor(AActor* a);
	static AActor* GetActor() {return selectedActor;}
	static void SetActorComponent(UActorComponent* comp);
	static UActorComponent* GetActorComponent() { return selectedComponent; }

	static void Clear() { selectedActor = nullptr; selectedComponent = nullptr; }

public:
	static inline FDynamicMulticastDelegate OnSelected;
	static inline FDynamicMulticastDelegate OnSelectedComponent;

private:
	static inline AActor* selectedActor = nullptr;
	static inline UActorComponent* selectedComponent = nullptr;
};