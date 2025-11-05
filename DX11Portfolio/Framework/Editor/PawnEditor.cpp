#include "Framework.h"
#include "PawnEditor.h"

PawnEditor::PawnEditor(APawn* target) : mTarget(target)
{
    RefreshBuffersFromTarget();
}

void PawnEditor::SetTarget(APawn* target)
{
	mTarget = target;
	RefreshBuffersFromTarget();
}

void PawnEditor::Update() {}

void PawnEditor::OnGUI()
{
	if (!mTarget)
		return;

    // --- 편집 시작 ---
    ImGui::PushID(mTarget);
    ImGui::SeparatorText("Pawn");

    // --- bUseControllerRotationPitch ---
    bool pitch = bUseControllerRotationPitch;
    if (ImGui::Checkbox("Use Controller Rotation Pitch", &pitch))
    {
        bUseControllerRotationPitch = pitch;
        mTarget->bUseControllerRotationPitch = pitch;
    }

    // --- bUseControllerRotationYaw ---
    bool yaw = bUseControllerRotationYaw;
    if (ImGui::Checkbox("Use Controller Rotation Yaw", &yaw))
    {
        bUseControllerRotationYaw = yaw;
        mTarget->bUseControllerRotationYaw = yaw;
    }

    // --- bUseControllerRotationRoll ---
    bool roll = bUseControllerRotationRoll;
    if (ImGui::Checkbox("Use Controller Rotation Roll", &roll))
    {
        bUseControllerRotationRoll = roll;
        mTarget->bUseControllerRotationRoll = roll;
    }

    ImGui::PopID();
}

void PawnEditor::RefreshBuffersFromTarget()
{
	if (mTarget == nullptr)
		return;

	bUseControllerRotationPitch = mTarget->bUseControllerRotationPitch;
	bUseControllerRotationRoll = mTarget->bUseControllerRotationRoll;
	bUseControllerRotationYaw = mTarget->bUseControllerRotationYaw;
}