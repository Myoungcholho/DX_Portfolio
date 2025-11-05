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

	void SetAssets(shared_ptr<const CPUMeshAsset> meshes, shared_ptr<const AnimationData> anim);
	void SetTrack(int index, bool loop, float rate);

public:
	void Tick() override;
	void FixedTick(double dt) override;

	UAnimInstance& GetAnimInstance() { return Anim; }

private:
	void AnimationUpdate(double dt);
	bool ShouldUpdateThisFrame(double dt);

private:
	UAnimInstance Anim;

	vector<Matrix> SkinningPalette;
	FRootMotionDelta accumRoot;			// 캐릭터 혹은 MovementComponent가, 월드 좌표에서 실제로 이동하는 주체가 들고 있어야 한다.

	Matrix accumRoot2 = Matrix();
	Vector3 prevPos = Vector3(0, 0, 0);

	// 4번째 루트모션 테스트 변수
	Quaternion prevRot = Quaternion();
	Vector3 outPos = Vector3();
	Quaternion outRot = Quaternion();
	Vector3 outScale = Vector3();

	//float acctime = 0.0f;
	//bool time = true;
	bool bUseRootMotion = false;

private:
	double AccumulatedAnimTime = 0.0;		// 애니메이션 시간 갱신용
	double accumulatedTime = 0.0;			// URO interval 체크용
};