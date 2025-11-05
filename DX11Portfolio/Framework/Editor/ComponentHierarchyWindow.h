#pragma once

class ComponentHierarchyWindow : public EditorWindow
{
public:
	ComponentHierarchyWindow();
	~ComponentHierarchyWindow();

public:
	void Initialize() override;
	void Update() override;
	void OnGUI() override;
	void Run() override;

private:
	void OnUpdateSeleted();
	FDelegateHandle ComponentHandle;

private:
	void DrawSceneCompNode(USceneComponent* sc, UActorComponent* selectedComp);
	void DrawNonSceneComp(UActorComponent* comp, UActorComponent* selectedComp);
};