#include "Framework.h"
#include "AnimationEditor.h"

AnimationEditor::AnimationEditor(USkeletalMeshComponent* target) : mTarget(target)
{
	RefreshBuffersFromTarget();
}

void AnimationEditor::SetTarget(USkeletalMeshComponent* target)
{
	mTarget = target;
	RefreshBuffersFromTarget();
}

void AnimationEditor::Update()
{

}

void AnimationEditor::OnGUI()
{
	if (!mTarget)
		return;

	UAnimInstance& anim = mTarget->GetAnimInstance();

	

	ImGui::SeparatorText("Animation Controller");

	// 현재 상태 표시
	string str = anim.GetCurrentClipName();
	ImGui::Text("Current Clip: %s", str.c_str());

	int curFrame = anim.GetCurrentFrame();
	int totalFrame = anim.GetTotalFrames();
	ImGui::Text("Frame: %d / %d", curFrame, totalFrame);
	if (ImGui::Button("Play"))
		anim.SetPause(false);

	ImGui::SameLine();
	if (ImGui::Button("Stop"))
		anim.SetPause(true);


	// --- 재생 컨트롤 ---
	vector<const char*> clipNames;
	clipNames.reserve(anim.GetAnimData()->clips.size());
	for (auto& clip : anim.GetAnimData()->clips)
	{
		clipNames.push_back(clip.name.c_str());
	}

	if (clipNames.empty())
	{
		ImGui::TextDisabled("No Clips Available");
		return;
	}
	ImGui::Separator();
	ImGui::Combo("Clip", &clipIndex, clipNames.data(), (int)clipNames.size());
	ImGui::Checkbox("Loop", &bLoop);
	ImGui::DragFloat("Rate", &playRate, 0.01f, 0.1f, 4.0f, "%.2fx");

	if (ImGui::Button("Change Animation"))
	{
		anim.SetPause(false);
		anim.PlayClip(clipIndex, bLoop, playRate);
	}

	ImGui::SeparatorText("Blending Transition");

	ImGui::Combo("Next Clip", &nextClipIndex, clipNames.data(), (int)clipNames.size());
	ImGui::DragFloat("Blend Duration", &blendDuration, 0.01f, 0.0f, 5.0f, "%.2fs");
	ImGui::Checkbox("Next Loop", &nextLoop);

	if (ImGui::Button("Start Blend"))
	{
		anim.CrossFadeTo(nextClipIndex, blendDuration, nextLoop, playRate);
		isBlending = true;
	}

	// --- 특정 프레임 미리보기 기능 ---
	ImGui::InputInt("Specific Frame", &specificFrame);

	if (ImGui::Button("Preview Frame"))
	{
		if (specificFrame >= 0 && specificFrame < anim.GetTotalFrames())
		{
			anim.PlayAtFrame(specificFrame);
		}
		else
		{
			// 안전한 범위 벗어나면 자동 보정
			specificFrame = clamp(specificFrame, 0, (int)anim.GetTotalFrames() - 1);
			anim.PlayAtFrame(specificFrame);
		}
	}
}

void AnimationEditor::RefreshBuffersFromTarget()
{
	if (!mTarget)
		return;

	UAnimInstance& anim = mTarget->GetAnimInstance();

	clipIndex = anim.GetClipIndex();
	bLoop = anim.GetLoop();
	playRate = anim.GetPlayRate();
	bPaused = anim.GetPause();

	isBlending = anim.IsBlending();
	nextClipIndex = anim.GetNextClipIndex();
	blendDuration = anim.GetBlendDuration();
}