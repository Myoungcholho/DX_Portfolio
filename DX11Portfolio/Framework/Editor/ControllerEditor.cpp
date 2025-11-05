#include "Framework.h"
#include "ControllerEditor.h"

ControllerEditor::ControllerEditor(AController* target) : mTarget(target)
{
	RefreshPawnList();
}

void ControllerEditor::SetTarget(AController* target)
{
	mTarget = target;
	RefreshPawnList();
}

void ControllerEditor::Update() { }

void ControllerEditor::OnGUI()
{
	if (!mTarget)
		return;

	ImGui::PushID(mTarget);
	ImGui::SeparatorText("Controller");

    // 현재 소유 중인 Pawn 표시
    APawn* currentPawn = mTarget->GetPawn();
    if (currentPawn)
        ImGui::Text("Current Pawn: %s", currentPawn->GetName().c_str());
    else
        ImGui::Text("Current Pawn: <None>");

    ImGui::Separator();


	ImGui::Text("Available Pawns:");
    // 현재 선택된 Pawn 이름 표시용
    std::string currentName = (mSelectedPawnIndex >= 0 && mSelectedPawnIndex < mPawnList.size())
        ? mPawnList[mSelectedPawnIndex]->GetName()
        : "<None>";

    // Combo (드롭다운 시작)
    if (ImGui::BeginCombo("##PawnListCombo", currentName.c_str()))
    {
        for (int i = 0; i < mPawnList.size(); ++i)
        {
            APawn* pawn = mPawnList[i];
            if (!pawn) continue;

            bool isSelected = (mSelectedPawnIndex == i);
            if (ImGui::Selectable(pawn->GetName().c_str(), isSelected))
            {
                mSelectedPawnIndex = i;
            }

            // 현재 선택된 항목이면 스크롤 포커스
            if (isSelected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }

    // --- Possess 버튼 ---
    if (mSelectedPawnIndex >= 0 && mSelectedPawnIndex < mPawnList.size())
    {
        if (ImGui::Button("Possess Selected Pawn"))
        {
            APawn* selectedPawn = mPawnList[mSelectedPawnIndex];
            if (selectedPawn)
            {
                mTarget->Possess(selectedPawn);
            }
        }
    }

	ImGui::PopID();
}

void ControllerEditor::RefreshPawnList()
{
	mPawnList.clear();

	if (!mTarget)
		return;

	UWorld* world = mTarget->GetWorld();
	if (!world)
		return;

	// 월드에서 Pawn들 전부 수집
	const auto& actors = world->GetActorsOf();
	for (AActor* actor : actors)
	{
		if (auto* pawn = dynamic_cast<APawn*>(actor))
			mPawnList.push_back(pawn);
	}
}
