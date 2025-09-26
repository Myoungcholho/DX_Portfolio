#include "Framework.h"
#include "USkeletalMeshComponent.h"

void USkeletalMeshComponent::SetAssets(const vector<PBRMeshData>& meshes, shared_ptr<const AnimationData> anim)
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

	acctime += dt;

	if (acctime >= 10.0f && time)
	{
		cout << "전환" << "\n";

		Anim.CrossFadeTo(1, 5, true);

		time = false;
	}

}

void USkeletalMeshComponent::FixedTick(double dt)
{
	if (AnimAsset->clips.empty())
		return;

	// 1. 로컬 포즈 계산
	Anim.Update(dt);

	// 2. 로컬 -> 컴포넌트
	const auto& local = Anim.GetLocalPose();
	AnimAsset->BuildComponentPoseFromLocal(
		local,
		Anim.GetFrame(),
		CSPose,
		accumRoot,
		prevPos
	);

	// 3. 팔레트 조립
	AnimAsset->ComposeSkinPalette(CSPose, bonesSnapshot);

}