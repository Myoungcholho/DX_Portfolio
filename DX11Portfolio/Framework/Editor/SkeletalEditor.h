#pragma once

class USkeletalMeshComponent;

class SkeletalEditor : public Editor
{
public:
	explicit SkeletalEditor(USkeletalMeshComponent* target = nullptr);

	void SetTarget(USkeletalMeshComponent* target);
	void OnGUI() override;

private:
	void RefreshBuffersFromTarget();		 // 타겟 값 → UI 버퍼
	void ApplyBuffersToTarget();			 // UI 버퍼 → 타겟 값

private:
	USkeletalMeshComponent* mTarget = nullptr;

	// Data


};