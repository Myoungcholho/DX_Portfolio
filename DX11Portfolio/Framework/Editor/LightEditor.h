#pragma once

class ULightComponent;
enum class LightType;

class LightEditor : public Editor
{
public:
	explicit LightEditor(ULightComponent* target = nullptr);

	void SetTarget(ULightComponent* target);
	bool IsValid() const { return mTarget != nullptr; }

	void Update() override;   // 훅(지금은 비움)
	void OnGUI() override;

private:
	void RefreshBuffersFromTarget();  // 타겟 → UI 버퍼
	void ApplyBuffersToTarget();      // UI 버퍼 → 타겟

private:
	ULightComponent* mTarget = nullptr;

	// Light Value Buffer
	bool enabled = true;
	Vector3 radiance = Vector3(1.0f);
	float fallOffStart = 0.0f;
	//Vector3 direction = Vector3(0.0f, 0.0f, 1.0f);
	float fallOffEnd = 10.0f;
	//Vector3 position = Vector3(0.0f, 0.0f, -2.0f);
	float spotPower = 100.0f;
	uint32_t type;

	// 드래그 중 실시간 반영
	bool mPreviewDuringEdit = true;
	// 편집 플래그
	bool mEditingAny = false;
};