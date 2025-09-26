#include "Framework.h"
#include "UAnimInstance.h"

void UAnimInstance::PlayClip(int index, bool loop, float rate)
{
    clipIndex = index;
    bLoop = loop;
    playRate = rate;

    frame = 0;
    frameAccum = 0.0;
}

void UAnimInstance::Update(double dt)
{
    if (!AnimData || AnimData->clips.empty())
        return;

    const AnimationClip& clip = AnimData->clips[clipIndex];
    const double ticksPerSec = (clip.ticksPerSec > 0) ? clip.ticksPerSec : 60.0;	// 클립이 1초에 도는 프레임 수
    const int clipNumKeys = (clip.numKeys > 0) ? clip.numKeys : 0;					// 프레임 개수

    // 정지하면 애니메이션 업데이트를 멈춤
    if (bPaused || ticksPerSec <= 0.0)
        return;

    frameAccum += dt * ticksPerSec * playRate;							// 델타 * 1초당 진행 속도 * 플레이 속도
    int step = (int)floor(frameAccum);									// 정수부분 추출
	if (step == 0)
	{
		if (localPose.empty())
			AnimData->EvaluateLocalPose(clipIndex, frame, localPose);
		return;
	}
    frameAccum -= step;													// 소수점 부분은 남겨서 다음 Tick에 사용

	if (bLoop)
	{
		if (clipNumKeys > 0)		// 키 프레임이 존재한다면
		{
			frame = (frame + step) % clipNumKeys;	// 키 프레임 개수로 나머지 연산을 해서 끝까지간 경우에 0번 프레임으로 복귀

			// -1 % 10 과 같은 연산 방지, 뒤로 갚는 루프의 경우 때문에
			if (frame < 0)
				frame += clipNumKeys;
		}
		else // %0 방지용
		{
			frame += step;

			if (frame < 0)
				frame = 0;
		}
	}
	else	// 한번만 재생
	{
		if (clipNumKeys > 0)
		{
			frame = clamp(frame + step, 0, clipNumKeys - 1);
		}
		else // %0 방지용
		{
			frame = max(0, frame + step);
		}
	}

	// 로컬 포즈 
	//AnimData->Update(clipIndex, frame);
	AnimData->EvaluateLocalPose(clipIndex, frame, localPose);
}

/// <summary>
/// 블렌딩 데이터 셋팅 함수
/// </summary>
/// <param name="nextIndex"> 전환될 클립 인덱스 </param>
/// <param name="durationSec"> 전환될 시간 </param>
/// <param name="loop"> 전환될 클립의 루프 여부 </param>
/// <param name="rate"> 전환될 클립의 애니메이션 속도 </param>
void UAnimInstance::CrossFadeTo(int nextIndex, float durationSec, bool loop, float rate)
{
	fromPose = localPose;

	nextClipIndex = nextIndex;
	nextLoop = loop;
	nextRate = rate;
	nextFrame = 0;
	nextAccum = 0.0;

	// 대상 포즈 1회 로컬 포즈 생성
	toPose.clear();
	if (AnimData)
		AnimData->EvaluateLocalPose(nextClipIndex, 0, toPose);

	blendDur = max(0.0001f, durationSec);
	blendT = 0.0f;
	bBlending = true;
}

/// <summary>
/// 두 클립의 로컬 포즈를 블렌딩해 전환 시 사용할 자연스러운 로컬 포즈 생성
/// </summary>
/// <param name="A">현재 진행 중 클립</param>
/// <param name="B">블랜드 대상 클립</param>
/// <param name="w">보간 실수 값</param>
/// <param name="Out">결과값 Output</param>
void UAnimInstance::BlendPose(const vector<AnimationClip::Key>& A, const vector<AnimationClip::Key>& B, float w, vector<AnimationClip::Key>& Out)
{
	assert(A.size() == 52 && B.size() == 52);
	const size_t n = A.size();
	if (Out.size() != n) Out.resize(n);

	w = std::clamp(w, 0.0f, 1.0f);
	for (int i = 0; i < n; ++i)
	{
		Out[i].pos = Vector3::Lerp(A[i].pos, B[i].pos, w);
		Out[i].scale = Vector3::Lerp(A[i].scale, B[i].scale, w);
		Out[i].rot = Quaternion::Slerp(A[i].rot, B[i].rot, w);
	}
}