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


	// 최종 스키닝 행렬 팔레트 스냅샷
	/*const int boneCount = (int)mAni.boneParents.size();
	if (boneCount <= 0)
		return;

	if ((int)bonesSnapshot.size() != boneCount)
		bonesSnapshot.reserve(boneCount);
	
	const int clipId = Anim.GetClipIndex();
	const int frame = Anim.GetFrame();

	for (int boneID = 0; boneID < boneCount; ++boneID)
	{
		bonesSnapshot[boneID] = mAni.Get(clipId, boneID, frame).Transpose();
	}*/
}