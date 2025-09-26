#pragma once

/// <summary>
/// 애니메이션 징검다리 역할로 UAnumInstace가 만든 본 팔레트 매트릭스 정보를
/// 베이스가 가지는 스냅샷에 저장하는 역할
/// 바인드 포즈 행렬을 통해 진짜 적용될 본 기준으로 매트릭스를 구성
/// </summary>
class USkeletalMeshComponent : public USkinnedMeshComponent
{
public:
	USkeletalMeshComponent() = default;

	void SetAssets(const vector<PBRMeshData>& meshes, shared_ptr<const AnimationData> anim);
	void SetTrack(int index, bool loop, float rate);

public:
	void FixedTick(double dt) override;

	UAnimInstance& GetAnimInstance() { return Anim; }

private:
	UAnimInstance Anim;

	vector<Matrix> CSPose;
	Matrix accumRoot = Matrix();
	Vector3 prevPos = Vector3(0, 0, 0);
};