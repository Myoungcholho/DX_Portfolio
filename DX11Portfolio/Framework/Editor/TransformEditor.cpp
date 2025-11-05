#include "Framework.h"
#include "TransformEditor.h"

TransformEditor::TransformEditor(USceneComponent* target) : mTarget(target)
{
	RefreshBuffersFromTarget();
}

void TransformEditor::SetTarget(USceneComponent* target)
{
	mTarget = target;
	mEditingPos = mEditingRot = mEditingScale = false;
	RefreshBuffersFromTarget();
}


void TransformEditor::Update()
{

}

void TransformEditor::OnGUI()
{
    if (!mTarget) { return; }

    // 편집 중이 아닐 때만 외부 변경을 버퍼에 반영
    if (!mEditingPos && !mEditingRot && !mEditingScale)
        RefreshBuffersFromTarget();

    ImGui::PushID(mTarget); // 컴포넌트별 고유 ID

    // 에디터 이름 사용(Inspector에서 SetName으로 넣어준 값)
    const char* title = GetName().empty() ? "Transform" : GetName().c_str();
    ImGui::SeparatorText(title);

    // --- Position ---
    {
        Vector3 v = mPosBuf;
        if (ImGui::DragFloat3("Position", &v.x, 0.01f)) {
            mPosBuf = v;
            if (mPreviewDuringEdit) {
                mTarget->SetRelativePosition(mPosBuf);
                //mTarget->UpdateWorldTransformRecursive();
            }
        }

        // 현재 위젯이 활성화된 순간 true, [마우스를 누른 직후, 키보드 포커스 잡힌 직후] 한 프레임만
        if (ImGui::IsItemActivated())             mEditingPos = true;
        // 편집이 끝나고 비활성화된 순간 한 프레임만 true
        if (ImGui::IsItemDeactivatedAfterEdit()) {
            if (!mPreviewDuringEdit) {
                mTarget->SetRelativePosition(mPosBuf);
                //mTarget->UpdateWorldTransformRecursive();
            }
            mEditingPos = false;
        }
    }

    // --- Rotation (degrees) ---
    {
        Vector3 v = mRotBuf;
        if (ImGui::DragFloat3("Rotation", &v.x, 0.5f)) {
            mRotBuf = v;
            if (mPreviewDuringEdit) {
                mTarget->SetRelativeRotation(mRotBuf); // degrees
                //mTarget->UpdateWorldTransformRecursive();
            }
        }
        if (ImGui::IsItemActivated())             mEditingRot = true;
        if (ImGui::IsItemDeactivatedAfterEdit()) {
            if (!mPreviewDuringEdit) {
                mTarget->SetRelativeRotation(mRotBuf); // degrees
                //mTarget->UpdateWorldTransformRecursive();
            }
            mEditingRot = false;
        }
    }

    // --- Scale ---
    {
        Vector3 v = mScaleBuf;
        if (ImGui::DragFloat3("Scale", &v.x, 0.01f)) {
            mScaleBuf = v;
            if (mPreviewDuringEdit) {
                mTarget->SetRelativeScale(mScaleBuf);
                //mTarget->UpdateWorldTransformRecursive();
            }
        }
        if (ImGui::IsItemActivated())             mEditingScale = true;
        if (ImGui::IsItemDeactivatedAfterEdit()) {
            if (!mPreviewDuringEdit) {
                mTarget->SetRelativeScale(mScaleBuf);
                //mTarget->UpdateWorldTransformRecursive();
            }
            mEditingScale = false;
        }
    }

    // Reset
    if (ImGui::Button("Reset")) {
        mPosBuf = Vector3(0, 0, 0);
        mRotBuf = Vector3(0, 0, 0);
        mScaleBuf = Vector3(1, 1, 1);
        ApplyBuffersToTarget();
    }

    ImGui::PopID();
}

void TransformEditor::RefreshBuffersFromTarget()
{
	if (!mTarget) return;
	mPosBuf = mTarget->GetRelativePosition();
	mRotBuf = mTarget->GetRelativeRotationEuler(); // degrees
	mScaleBuf = mTarget->GetRelativeScale();
}

void TransformEditor::ApplyBuffersToTarget()
{
	if (!mTarget) return;

	//mTarget->SetRelativePosition(mPosBuf);
	//mTarget->SetRelativeRotation(mRotBuf); // degrees 버전 사용
	//mTarget->SetRelativeScale(mScaleBuf);
	//mTarget->UpdateWorldTransformRecursive();

    FTransformUpdateCommand cmd;
    cmd.target = mTarget;
    cmd.position = mPosBuf;
    cmd.rotation = mRotBuf;
    cmd.scale = mScaleBuf;

    FEditorCommandQueue::Enqueue(cmd);
}