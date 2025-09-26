#pragma once

/// <summary>
/// 어떤 클립을 몇 프레임 데이터를 쓸지 결정하고,
/// 키 프레임데이터로 부모-자식간 계층 연산을 통해 본 팔레트 정보를 구성하는 역할
/// </summary>
class UAnimInstance
{
public:
	UAnimInstance() = default;
	virtual ~UAnimInstance() = default;

	void Initialize(shared_ptr<const AnimationData> animData) { AnimData = animData; }

	void PlayClip(int index, bool loop = true, float rate = 1.0f);
	void Pause(bool b) { bPaused = b; }
	void SetPlayRate(float r) { playRate = r; }
	void SetLoop(bool b) { bLoop = b; }

	void Update(double dt);

	const AnimationData* GetAnimData() { return AnimData.get(); }
	int GetClipIndex() const { return clipIndex; }
	int GetFrame() const { return frame; }
	const vector<AnimationClip::Key>& GetLocalPose() const { return localPose; }

public:
	void CrossFadeTo(int nextIndex, float durationSec, bool loop = true, float rate = 1.0f);

private:
	shared_ptr<const AnimationData> AnimData;

	// 외부 변경 데이터
	int clipIndex = 0;
	float playRate = 1.0f;
	bool bLoop = true;

	// 내부 사용 데이터
	int frame = 0;
	double frameAccum = 0.0;
	bool bPaused = false;

	vector<AnimationClip::Key> localPose;
private:
	// 블렌딩 상태 멤버
	bool bBlending = false;
	float blendDur = 0.0f;						// Blend 길이
	float blendT = 0.0f;						// Blend 경과 시간

	int nextClipIndex = -1;
	bool nextLoop = true;
	float nextRate = 1.0f;
	int nextFrame = 0;
	double nextAccum = 0.0;

	vector<AnimationClip::Key> fromPose;		// 현재 클립 포즈
	vector<AnimationClip::Key> toPose;			// 대상 클립 포즈

	void BlendPose(const std::vector<AnimationClip::Key>& A,
		const std::vector<AnimationClip::Key>& B,
		float w,
		std::vector<AnimationClip::Key>& Out);
};