#include "Framework.h"
#include "EditorSelection.h"

void EditorSelection::SetActor(AActor* a)
{
	selectedActor = a;
	selectedComponent = nullptr;

	if (OnSelected.IsBound())
		OnSelected.Broadcast();
}

void EditorSelection::SetActorComponent(UActorComponent* comp)
{
	selectedComponent = comp;

	if (OnSelectedComponent.IsBound())
		OnSelectedComponent.Broadcast();
}
