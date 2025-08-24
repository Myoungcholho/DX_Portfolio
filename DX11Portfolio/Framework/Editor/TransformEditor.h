#pragma once

class USceneComponent;

class TransformEditor : public Editor
{
public:
	explicit TransformEditor(USceneComponent* target = nullptr);

	void SetTarget(USceneComponent* target);
	bool IsValid() const { return mTarget != nullptr; }

	void Update() override;					 // 현재는 비움(훅)
	void OnGUI() override;

private:
	void RefreshBuffersFromTarget();		 // 타겟 값 → UI 버퍼
	void ApplyBuffersToTarget();			 // UI 버퍼 → 타겟 값

private:
	USceneComponent* mTarget = nullptr;

	// 좌표 버퍼
	Vector3 mPosBuf{ 0,0,0 };
	Vector3 mRotBuf{ 0,0,0 };
	Vector3 mScaleBuf{ 1,1,1 };

	// 드래그 중인지 확인용 flag
	bool mEditingPos = false;
	bool mEditingRot = false;
	bool mEditingScale = false;

	bool mPreviewDuringEdit = true;			// 드래그 중 값 실시간 반영 여부
};