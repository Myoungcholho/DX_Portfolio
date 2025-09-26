#include "Framework.h"
#include "ComponentHierarchyWindow.h"

// ---- 내부 유틸
namespace 
{
	static const char* kPayloadSceneComp = "SCOMP";

	inline bool StrICaseContains(const std::string& s, const char* pat) {
		if (!pat || !*pat) return true;
		std::string a = s, b = pat;
		std::transform(a.begin(), a.end(), a.begin(), ::tolower);
		std::transform(b.begin(), b.end(), b.begin(), ::tolower);
		return a.find(b) != std::string::npos;
	}

	// b 가 a의 후손인지 검사(사이클 방지)
	bool IsAncestor(USceneComponent* a, USceneComponent* b) {
		if (!a || !b) return false;
		auto* p = b->GetParent();               // ← GetParent() 이름은 프로젝트에 맞게
		while (p) {
			if (p == a) return true;
			p = p->GetParent();
		}
		return false;
	}
}

ComponentHierarchyWindow::ComponentHierarchyWindow()
{
	SetName("ActorOutliner");
	SetSize(ImVec2(300, 600));
}

ComponentHierarchyWindow::~ComponentHierarchyWindow()
{
	if (m_ComponentHandle.IsValid())
		EditorSelection::OnSelected.Remove(m_ComponentHandle);
}

void ComponentHierarchyWindow::Initialize() 
{
	m_ComponentHandle = EditorSelection::OnSelected.AddDynamic(this, &ComponentHierarchyWindow::OnUpdateSeleted, "ComponentHierarchyWindow::OnUpdateSeleted");
}

void ComponentHierarchyWindow::Run()
{
	bool active = (bool)GetState();
	ImGui::Begin(GetName().c_str(), &active, GetFlag());

	Update();
	OnGUI();

	ImGui::End();
}

void ComponentHierarchyWindow::Update() {}

void ComponentHierarchyWindow::OnGUI()
{
	// -- 선택된 액터 가져오기 --
	AActor* actor = EditorSelection::GetActor();
	if (!actor) {
		ImGui::TextDisabled("No actor selected.");
		return;
	}

	// 액터 라벨
#if IMGUI_VERSION_NUM >= 18900
	ImGui::SeparatorText(actor->GetName().c_str());
#else
	ImGui::TextUnformatted(actor->GetName().c_str());
	ImGui::Separator();
#endif

	UActorComponent* selectedComp = EditorSelection::GetActorComponent();


	// 1) 루트 씬 컴포넌트부터 트리표시
	auto rootSP = actor->GetRootComponent();
	if (rootSP) 
	{
		DrawSceneCompNode(rootSP.get(), selectedComp);
	}

	// 2) 루트에 속하지 않는(또는 Non-Scene) 컴포넌트들을 상단 리스트로 추가
	for (auto& sp : actor->GetComponents()) 
	{
		UActorComponent* comp = sp.get();		// shared_ptr -> raw_ptr

		if (auto sc = dynamic_cast<USceneComponent*>(comp)) 
		{
			if (sc == rootSP.get())				// 자기 자신이라면 건너뛰기
				continue;
			if(sc->GetParent() != nullptr)		// 부모가 있다면 그려졌으므로 건너뛰기
				continue;

			// 새 씬컴포넌트를 만들었는데 루트에 안 붙여졌거나, Detach하고 안붙었거나.. 그런 예외이나 내 경우에는 아직 그럴 경우가 없긴하다.
			DrawSceneCompNode(sc, selectedComp);// 액터가 소유하되 루트컴포넌트의 자식이 아닌 컴포넌트 Draw
			continue;
		}

		DrawNonSceneComp(comp, selectedComp);	// 소위 ActorComponent인것들
	}
}

/// <summary>
/// SceneComponent를 상속한 컴포넌트들의 계층구조 Tree Draw 작업
/// </summary>
void ComponentHierarchyWindow::DrawSceneCompNode(USceneComponent* sc, UActorComponent* selectedComp)
{
	if (sc == nullptr)
		return;

	// 화살표 눌렀을 때 펼침 | 선택 하이라이트/클릭 영역 가로 전체로 확장
	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanFullWidth;
	
	const bool hasChildren = !sc->GetChildren().empty();

	// 자식이 없다면
	if (!hasChildren)
	{
		// 자식 없는 말단 노드 | Push하지 않음으로 TreePop() 호출 안하게
		flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
	}

	// 만약 선택된 컴포넌트라면
	if (selectedComp == sc)
	{
		// 선택된 컴포넌트 하이라이트
		flags |= ImGuiTreeNodeFlags_Selected;
	}

	// 트리 노드 실제로 그림, 이번 프레임에 펼쳐졌다면 true를 반환
	const bool open = ImGui::TreeNodeEx((void*)sc, flags, "%s", sc->mName.c_str());

	// 방금 그린 항목을 클릭했다면 액터 컴포넌트 등록
	if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
		EditorSelection::SetActorComponent(sc);

	// 리프면 push가 없었으니 pop하지 않기 위해 return
	if (!hasChildren) 
		return;

	// 펼쳐진 상태라면 재귀로 자식을 그리기
	if (open) 
	{
		for (auto* child : sc->GetChildren())
			DrawSceneCompNode(child, selectedComp);
		ImGui::TreePop();
	}
}

/// <summary>
/// SceneComponent를 상속하지 않는, ActorComponent를 그리기 위한 기능
/// </summary>
void ComponentHierarchyWindow::DrawNonSceneComp(UActorComponent* comp, UActorComponent* selectedComp)
{
	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_Leaf
		| ImGuiTreeNodeFlags_NoTreePushOnOpen
		| ImGuiTreeNodeFlags_SpanFullWidth;
	if (selectedComp == comp)
		flags |= ImGuiTreeNodeFlags_Selected;

	ImGui::TreeNodeEx((void*)comp, flags, "%s", comp->mName.c_str());
	if (ImGui::IsItemClicked())
		EditorSelection::SetActorComponent(comp);
}

/// <summary>
/// Selete된 대상 Actor가 변경될때마다 호출
/// </summary>
void ComponentHierarchyWindow::OnUpdateSeleted()
{
	// 이걸 할 필요가 있나? 
	// OnGUI에서 EditorSelection의 선택된 Actor를 바탕으로 컴포넌트 트리를 구성하면 되는게 아닌가?
	// 뭐 나중에 필요할때가 있겠지..
}