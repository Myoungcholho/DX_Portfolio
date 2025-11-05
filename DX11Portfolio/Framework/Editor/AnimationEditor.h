#pragma once

class USkeletalMeshComponent;
class UAnimInstance;

class AnimationEditor : public Editor
{
public:
	explicit AnimationEditor(USkeletalMeshComponent* target = nullptr);

	void SetTarget(USkeletalMeshComponent* target);
	bool IsValid() const { return mTarget != nullptr; }

	void Update() override;
	void OnGUI() override;

private:
	void RefreshBuffersFromTarget();			// 스켈레톤이 가지는 에디터 값 불러오기

private:
	USkeletalMeshComponent* mTarget = nullptr;

	// 재생 컨트롤
	int clipIndex = 0;
	bool bLoop = true;
	float playRate = 1.0f;
	bool bPaused = false;
	int specificFrame = -1;						// 정적 포즈 확인용

	// 블렌딩
	int nextClipIndex = -1;
	float blendDuration = 0.2f;
	bool nextLoop = false;
	bool isBlending = false;					// 블렌딩 중에는 막으려고 내부가 가짐

	bool previewDuringEdit = true;				// 드래그 중에 실시간 반영 여부

};