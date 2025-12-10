#include "Framework.h"
#include "UAnimInstance.h"

/// <summary>
/// 애니메이션 클립 실행
/// </summary>
/// <param name="index"></param>
/// <param name="loop"></param>
/// <param name="rate"></param>
void UAnimInstance::PlayClip(int index, bool loop, float rate)
{
	if (AnimData == nullptr)
		return;

	if (index < 0 || index >= AnimData->clips.size())
		return;

    clipIndex = index;
    bLoop = loop;
    playRate = rate;

    frame = 0;
    frameAccum = 0.0;
}

/// <summary>
/// 특정 프레임에서 애니메이션을 정지
/// </summary>
/// <param name="startFrame"></param>
/// <param name="loop"></param>
/// <param name="rate"></param>
void UAnimInstance::PlayAtFrame(int startFrame)
{
	if (AnimData == nullptr || AnimData->clips.empty())
		return;

	frame = clamp(startFrame, 0, AnimData->clips[clipIndex].numKeys - 1);
	frameAccum = 0.0;
	bPaused = true;

	AnimData->EvaluateLocalPose(clipIndex, frame, frameAccum, localPose);
}

void UAnimInstance::Update(double dt)
{
    if (AnimData == nullptr || AnimData->clips.empty())
        return;

	if (clipIndex < 0 || clipIndex >= AnimData->clips.size())
		return;


    const AnimationClip& clip = AnimData->clips[clipIndex];
    const double ticksPerSec = (clip.ticksPerSec > 0) ? clip.ticksPerSec : 60.0;	// 클립이 1초에 도는 프레임 수
    const int clipNumKeys = (clip.numKeys > 0) ? clip.numKeys : 0;					// 프레임 개수

    // 정지하면 애니메이션 업데이트를 멈춤
    if (bPaused || ticksPerSec <= 0.0)
        return;

	// ---- 프레임 진행(현재 파이프) ------------------------------------------------------------
    frameAccum += dt * ticksPerSec * playRate;										// 델타 * 1초당 진행 속도 * 플레이 속도
    int step = (int)floor(frameAccum);												// 정수부분 추출
	if (step > 0)
	{
		frameAccum -= step;
		if (bLoop)
		{
			if (clipNumKeys > 0) 
			{
				frame = (frame + step) % clipNumKeys;
				if (frame < 0) frame += clipNumKeys;
			}
			else 
			{
				frame += step;
				if (frame < 0) frame = 0;
			}
		}
		else
		{
			if (clipNumKeys > 0)
			{
				frame = clamp(frame + step, 0, clipNumKeys - 1);

				// 마지막 프레임에 도달하면 정지
				if (frame >= clipNumKeys - 1)
				{
					frame = clipNumKeys - 1;
					frameAccum = 0.0;
					return;
				}
			}
			else
				frame = max(0, frame + step);
		}
	}

	AnimData->EvaluateLocalPose(clipIndex, frame, frameAccum, localPose);

	// ---- 블렌딩 처리 ------------------------------------------------------------------------
	if (bBlending && nextClipIndex >= 0)
	{
		const AnimationClip& nextClip = AnimData->clips[nextClipIndex];
		const double nextTicksPerSec = (nextClip.ticksPerSec > 0) ? nextClip.ticksPerSec : 60.0;
		const int nextNumKeys = (nextClip.numKeys > 0) ? nextClip.numKeys : 0;

		nextAccum += dt * nextTicksPerSec * nextRate;
		int nstep = (int)floor(nextAccum);

		if (nstep > 0)
		{
			nextAccum -= nstep;

			if (nextLoop)
			{
				if (nextNumKeys > 0)
				{
					nextFrame = (nextFrame + nstep) % nextNumKeys;
					if (nextFrame < 0) nextFrame += nextNumKeys;
				}
				else
				{
					nextFrame += nstep;
					if (nextFrame < 0) nextFrame = 0;
				}
			}
			else
			{
				if (nextNumKeys > 0) nextFrame = clamp(nextFrame + nstep, 0, nextNumKeys - 1);
				else nextFrame = max(0, nextFrame + nstep);
			}
		}

		// 두 포즈 결과를 추출하고 Blend
		fromPose = localPose;													// 매 프레임 최신 현재 포즈
		AnimData->EvaluateLocalPose(nextClipIndex, nextFrame, nextAccum, toPose);

		// 시간 기반 가중치 (smoothstep)
		blendT = min(blendT + (float)dt, blendDur);
		float u = (blendDur > 0.0001f) ? (blendT / blendDur) : 1.0f;
		float w = u * u * (3.f - 2.f * u);

		// 보간 결과를 이번 프레임의 출력 포즈로 사용
		BlendPose(fromPose, toPose, w, localPose);

		// 전환 종료 처리
		if (blendT >= blendDur)
		{
			// 대상 클립으로 스위치
			clipIndex = nextClipIndex;
			bLoop = nextLoop;
			playRate = nextRate;

			// 대상 파이프 진행도를 현재 파이프에 이양
			frame = nextFrame;
			frameAccum = nextAccum;

			// 블렌딩 해제 및 클린업
			bBlending = false;
			nextClipIndex = -1;
			toPose.clear();
			fromPose.clear();

			prevFrame = -1;					// 권위 prev 리셋
		}

	}
}

string UAnimInstance::GetCurrentClipName() const
{
	if (AnimData == nullptr)
		return "AnimData is nullptr";

	if (clipIndex < 0 || clipIndex >= AnimData->clips.size())
		return "clipIndex out of range";

	return AnimData->clips[clipIndex].name;
}

float UAnimInstance::GetCurrentFrame() const
{
	return frame;
}

/// <summary>
/// 진행중인 애니메이션의 전체 길이 반환
/// </summary>
/// <returns></returns>
float UAnimInstance::GetTotalFrames() const
{
	if (AnimData == nullptr)
		return -1;

	if (clipIndex < 0 || clipIndex >= AnimData->clips.size())
		return -1;

	return AnimData->clips[clipIndex].numKeys;
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
	if (AnimData == nullptr)
		return;

	fromPose = localPose;

	nextClipIndex = nextIndex;
	nextLoop = loop;
	nextRate = rate;
	nextFrame = 0;
	nextAccum = 0.0;

	// 대상 포즈 1회 로컬 포즈 생성
	toPose.clear();
	
	AnimData->EvaluateLocalPose(nextClipIndex, 0, nextAccum, toPose);

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
	assert(A.size() == B.size());

	const size_t n = A.size();
	if (Out.size() != n) Out.resize(n);

	w = clamp(w, 0.0f, 1.0f);
	for (int i = 0; i < n; ++i)
	{
		Out[i].pos = Vector3::Lerp(A[i].pos, B[i].pos, w);
		Out[i].scale = Vector3::Lerp(A[i].scale, B[i].scale, w);
		Out[i].rot = MathHelper::SlerpSafe(A[i].rot, B[i].rot, w);
		//Out[i].rot = Quaternion::Lerp(A[i].rot, B[i].rot, w);
	}
}