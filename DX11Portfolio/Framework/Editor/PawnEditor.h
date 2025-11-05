#pragma once

class APawn;

class PawnEditor : public Editor
{
public:
	explicit PawnEditor(APawn* target = nullptr);

	void SetTarget(APawn* target);
	bool IsValid() const { return mTarget != nullptr; }

	void Update() override;
	void OnGUI() override;
private:
	void RefreshBuffersFromTarget();  // Å¸°Ù ¡æ UI ¹öÆÛ

private:
	APawn* mTarget = nullptr;

	bool bUseControllerRotationYaw = false;
	bool bUseControllerRotationPitch = false;
	bool bUseControllerRotationRoll = false;
};