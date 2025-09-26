#include "Framework.h"
#include "WorldOutlinerWindow.h"
#include <Shlwapi.h>
#pragma comment(lib, "Shlwapi.lib")

WorldOutlinerWindow::WorldOutlinerWindow()
{
	SetName("World Outliner");
	SetSize(ImVec2(300, 600));
}

WorldOutlinerWindow::~WorldOutlinerWindow()
{
}

void WorldOutlinerWindow::Initialize(){}

void WorldOutlinerWindow::Update(){}

void WorldOutlinerWindow::Run()
{
	bool active = (GetState() == eState::Active);
	ImGui::Begin(GetName().c_str(), &active, GetFlag());

	Update();
	OnGUI();

	ImGui::End();
}

void WorldOutlinerWindow::OnGUI()
{
	if (!m_world)
	{
		ImGui::TextDisabled("No world.");
		return;
	}
	
	// --- 검색 ---
	ImGui::SetNextItemWidth(-1);
	if (ImGui::InputTextWithHint("##search", "Search actors...", mSearch, sizeof(mSearch))) 
	{
		// 필요 시 검색 갱신 로직
	}

	ImGui::Checkbox("Hierarchy", &mUseHierarchy);
	ImGui::Separator();

	const auto& actors = m_world->GetActorsOf();

	if (mUseHierarchy) {
		// 트리 모드: 루트만 찍고, 재귀로 자식 출력
		for (AActor* a : actors)
		{
			if (!a->GetParent())             // 부모가 없는 애만 시작점
				DrawActorNode(a);            // 내부에서 children 재귀
		}
	}
	else {
		// 리스트 모드: 부모/자식 무시, 전부 1레벨로
		for (AActor* a : actors)
		{
			DrawActorFlat(a);
		}
	}
}

/// <summary>
/// name : 비교대상, filter : 필터문자
/// </summary>
static bool PassFilter(const char* name, const char* filter) 
{
	if (!filter || !filter[0]) return true;
	return StrStrIA(name, filter) != nullptr;
}

void WorldOutlinerWindow::DrawActorNode(AActor* a)
{
	if (a == nullptr)
		return;

	// 검색
	const string& name = a->GetName();
	if (!PassFilter(name.c_str(), mSearch))
	{

		return;
	}

	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanFullWidth;	// 왼쪽 화살표 클릭으로 펼침 | 항목의 클릭 영역을 가로 전체로 확장
	if (EditorSelection::GetActor() == a) flags |= ImGuiTreeNodeFlags_Selected;							// 선택된 Actor면 선택(하이라이트) 상태 플래그 추가
	
	bool hasChildren = false;																		// 자식 보유 여부
	if (!hasChildren) flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;		// 화살표 없이 리프 | TreeNodeEx호출 후 TreePop 호출하지 않음

	bool open = ImGui::TreeNodeEx((void*)a, flags, "%s", name.c_str());										// (노드 고유 ID, 트리 flag, 표시할 라벨)

	if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen()) {									// 항목 클릭 시 그 클릭이 펼침/접힙 토글을 유발한 클릭이 아니라면 선택
		EditorSelection::SetActor(a);																	// 객체를 선택
	}

	if (ImGui::BeginPopupContextItem()) {														// 컨텍스트 팝업 메뉴(우클릭)
		if (ImGui::MenuItem("Select")) {
			EditorSelection::SetActor(a);
		}
		if (ImGui::MenuItem("Delete")) {															
			// TODO: 월드에서 제거
		}
		ImGui::EndPopup();																			
	}

	if (open && hasChildren) {																		// 노드가 열렸고 자식이 있다면 자식들 그리기
		// for (Actor* child : a->GetChildren()) DrawActorNode(child);								//
		ImGui::TreePop();																			// 자식이 있는 일반 노드일 때만 호출
	}
}

void WorldOutlinerWindow::DrawActorFlat(AActor* a)
{
	if (!a) return;

	// 검색
	const string& name = a->GetName();
	if (!PassFilter(name.c_str(), mSearch)) return;													// 검색어에 매치되지 않는다면 그리지 않음


	bool selected = (EditorSelection::GetActor() == a);													// 현재 선택 상태 여부

	ImGui::PushID(a);
	if (ImGui::Selectable(name.c_str(), selected)) {												// 한 줄짜리 선택 항목으로 클릭 시 true
		EditorSelection::SetActor(a);																	// 이 액터를 현재 선택으로 설정
	}
	ImGui::PopID();

	if (ImGui::BeginPopupContextItem()) 
	{
		if (ImGui::MenuItem("Select")) 
			EditorSelection::SetActor(a);
		if (ImGui::MenuItem("Delete")) { /* TODO */ }
		if (ImGui::MenuItem("Rename")) { /* TODO */ }
		ImGui::EndPopup();
	}
}