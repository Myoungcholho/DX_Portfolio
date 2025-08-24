#pragma once

class UWorld;
class AActor;

class WorldOutlinerWindow : public EditorWindow
{
public:
	WorldOutlinerWindow();
	~WorldOutlinerWindow();

	void Initialize() override;
	void Update() override;
	void OnGUI() override;
	void Run() override;

public:
	void SetWorld(UWorld* world) { m_world = world; }

private:
	void DrawActorNode(AActor* a);
	void DrawActorFlat(AActor* a);		// 계층구조 없으면 이걸로

private:
	bool mUseHierarchy = false;			// 부모-자식 트리 지원 여부
	char mSearch[128] = { 0 };			// 검색 필터

private:
	UWorld* m_world;
};