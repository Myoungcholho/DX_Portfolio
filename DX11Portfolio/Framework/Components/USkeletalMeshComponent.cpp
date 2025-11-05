#include "Framework.h"
#include "USkeletalMeshComponent.h"

void USkeletalMeshComponent::SetAssets(shared_ptr<const CPUMeshAsset> meshes, shared_ptr<const AnimationData> anim)
{
	USkinnedMeshComponent::SetAssets(meshes, anim);

	Anim.Initialize(AnimAsset);
	Anim.PlayClip(0, true, 1.0f);
}

void USkeletalMeshComponent::SetTrack(int index, bool loop, float rate)
{
	Anim.PlayClip(index, loop, rate);
}

void USkeletalMeshComponent::Tick()
{
	float dt = CTimer::Get()->GetDeltaTime();

	//acctime += dt;

	//if (acctime >= 10.0f && time)
	//{
	//	//cout << "전환" << "\n";

	//	//Anim.CrossFadeTo(1, 5, true);

	//	time = false;
	//}

	AnimationUpdate(dt);
}

void USkeletalMeshComponent::AnimationUpdate(double dt)
{
	AccumulatedAnimTime += dt;

	// 1. URO 체크
	if (ShouldUpdateThisFrame(dt) == false)
		return;

	if (AnimAsset->clips.empty())
		return;

	// 1. 로컬 포즈 계산 [내부에서 키 프레임 보간과, 두 클립의 블렌딩도 수행함]
	Anim.Update(AccumulatedAnimTime);
	AccumulatedAnimTime = 0.0;
	
	// 1. 권위 클립/프레임 선택
	const int authClipIndex = Anim.GetClipIndex();
	const int authFrame = Anim.GetFrame();
	
	const AnimationClip::Key rootStart = AnimAsset->GetRootKeyStart(authClipIndex);
	const AnimationClip::Key rootEnd = AnimAsset->GetRootKeyEnd(authClipIndex);

	// 2. 로컬 -> 컴포넌트
	const vector<AnimationClip::Key>& local = Anim.GetLocalPose();

	AnimAsset->BuildBonePaletteFromLocal(
		local,
		Anim.GetFrame(),
		SkinningPalette,
		accumRoot2,
		prevPos
	);

	/*AnimAsset->BuildComponentPoseFromLocal4(
		local,
		Anim.GetFrame(),
		CSPose,
		outPos, outRot, outScale,
		prevPos, prevRot
	);*/

	if (bUseRootMotion)
	{
		Transform T = GetRelativeTransform();

		// 이동 (현재 회전 기준으로 변환)
		Vector3 worldDelta = Vector3::Transform(outPos, Matrix::CreateFromQuaternion(T.GetRotationQuat()));
		T.SetPosition(T.GetPosition() + worldDelta);

		// 회전 누적
		Quaternion newRot = T.GetRotationQuat() * outRot;
		newRot.Normalize();
		T.SetRotation(newRot);

		SetRelativeTransform(T);
	}

	// 3. 팔레트 조립
	AnimAsset->ComposeSkinPalette(SkinningPalette, bonesSnapshot);
}

bool USkeletalMeshComponent::ShouldUpdateThisFrame(double dt)
{
	const double targetInterval = 1.0 / 75.0;	// 0.0133

	double interval = targetInterval;

	Vector3 cameraPos = CContext::Get()->GetCamera()->GetPosition();
	Vector3 actorPos = GetActorWorldPosition();

	float dist = (cameraPos - actorPos).Length();
	
	if (dist > 10.0f) interval *= 2.0;
	if (dist > 20.0f) interval *= 4.0;

	accumulatedTime += dt;

	//cout << accumulatedTime << "\n";

	if (accumulatedTime >= interval)
	{
		accumulatedTime = fmod(accumulatedTime, interval);		// 드리프트 현상 방지를 위해 넘친 시간 보존
		return true;
	}

	return false;
}

void USkeletalMeshComponent::FixedTick(double dt)
{


}

//AnimAsset->BuildComponentPoseFromLocal3(
//	local,
//	authFrame,
//	rootStart, rootEnd,
//	CSPose,
//	accumRoot,
//	Anim.prevRootPos,
//	Anim.prevRootRot,
//	Anim.prevFrame,
//	0.002
//);

/*AnimAsset->BuildComponentPoseFromLocal2
(
	local,
	Anim.GetFrame(),
	CSPose,
	accumRoot,
	Anim.prevRootPos,
	Anim.prevRootRot,
	Anim.prevFrame
);*/